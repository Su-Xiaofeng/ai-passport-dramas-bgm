#!/usr/bin/env python3
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
import wave

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("pack_bgm", ROOT / "tools/pack_bgm.py")
pack_bgm = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pack_bgm)


class PackTest(unittest.TestCase):
    def manifest(self, root):
        path = root / "playlist.json"
        path.write_text(json.dumps({"sample_rate": 16000, "tracks": [
            {"title": f"Track {i}", "file": f"{i}.wav", "start": 0, "duration": 0.1}
            for i in range(6)]}))
        return path

    def test_known_ima_codes(self):
        self.assertEqual(pack_bgm.encode([0,1,4,8,15,27,47,88]), bytes.fromhex("10325476"))
        self.assertEqual(pack_bgm.encode([0,0,0]), b"\0\0")

    def test_missing_audio_is_explicit_and_release_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            path = self.manifest(root)
            output = root / "data.c"
            report = pack_bgm.pack(path, output)
            self.assertTrue(all(not t["loaded"] for t in report))
            before = output.read_bytes()
            with self.assertRaisesRegex(ValueError, "Missing audio"):
                pack_bgm.pack(path, output, require_all=True)
            self.assertEqual(before, output.read_bytes())

    def test_ffmpeg_pack_six_real_files(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            path = self.manifest(root)
            for i in range(6):
                with wave.open(str(root / f"{i}.wav"), "wb") as wav:
                    wav.setparams((1,2,16000,1600,"NONE","not compressed"))
                    wav.writeframes(b"\0\0" * 1600)
            report = pack_bgm.pack(path, root / "data.c", require_all=True)
            self.assertEqual(len(report), 6)
            self.assertTrue(all(t["samples"] == 1600 and t["bytes"] == 800 for t in report))

    def test_reject_unsafe_path_and_invalid_trim(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = self.manifest(Path(tmp))
            manifest = json.loads(path.read_text())
            manifest["tracks"][0]["file"] = "../outside.mp3"
            path.write_text(json.dumps(manifest))
            with self.assertRaisesRegex(ValueError, "inside"):
                pack_bgm.read_manifest(path)
            manifest["tracks"][0]["file"] = "0.wav"
            manifest["tracks"][0]["duration"] = 0
            path.write_text(json.dumps(manifest))
            with self.assertRaisesRegex(ValueError, "duration"):
                pack_bgm.read_manifest(path)


if __name__ == "__main__":
    unittest.main()
