"""Build the TTK4155 code submission zip.

Usage (from the repository root):
    python3 make_submission.py 7        ->  group_07_ttk4155_h26.zip

Packs README.md, node1/ and node2/ into group_XX_ttk4155_h26/ inside the zip, so extracting it
gives one folder with that name. Only files known to git (committed or new, not ignored) are
included, and PDFs, build output and the unmodified handout folder node2/sam/ are left out.
"""

import subprocess
import sys
import zipfile
from pathlib import Path

SIZE_LIMIT = 200 * 1024     # the assignment says > 200 kB probably means something was overlooked

INCLUDE_ROOTS = ("README.md", "node1/", "node2/")
EXCLUDE_PREFIXES = ("node2/sam/",)
EXCLUDE_PARTS = ("build",)
EXCLUDE_SUFFIXES = (".pdf", ".o", ".d", ".hex", ".elf", ".bin", ".out", ".map", ".zip")
EXCLUDE_NAMES = (".gitignore", "sram_test.c")   # sram_test.c: copied from the lab manual


def wanted(path: str) -> bool:
    if not path.startswith(INCLUDE_ROOTS):
        return False
    if path.startswith(EXCLUDE_PREFIXES):
        return False
    p = Path(path)
    if any(part in EXCLUDE_PARTS for part in p.parts):
        return False
    if p.suffix.lower() in EXCLUDE_SUFFIXES or p.name in EXCLUDE_NAMES:
        return False
    return True


def main() -> int:
    if len(sys.argv) != 2 or not sys.argv[1].isdigit():
        print(__doc__)
        return 1

    name = f"group_{int(sys.argv[1]):02d}_ttk4155_h26"
    root = Path(__file__).resolve().parent

    # Committed files plus new files that are not ignored
    files = subprocess.run(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard"],
        cwd=root, capture_output=True, text=True, check=True,
    ).stdout.splitlines()
    files = sorted(f for f in files if wanted(f) and (root / f).is_file())

    if "README.md" not in files:
        print("Error: README.md is missing in the repository root")
        return 1

    first_line = (root / "README.md").read_text(encoding="utf-8").splitlines()[0].strip()
    if not first_line.startswith("http"):
        print(f"Error: the first line of README.md must be the Git URL, got: {first_line!r}")
        return 1

    zip_path = root / f"{name}.zip"
    total = 0
    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as z:
        for f in files:
            z.write(root / f, f"{name}/{f}")
            total += (root / f).stat().st_size

    for f in files:
        print(f"  {f}")
    print(f"\n{len(files)} files, {total / 1024:.1f} kB uncompressed, "
          f"{zip_path.stat().st_size / 1024:.1f} kB zipped -> {zip_path.name}")
    if total > SIZE_LIMIT:
        print("Warning: more than 200 kB, check that nothing was overlooked")
    return 0


if __name__ == "__main__":
    sys.exit(main())
