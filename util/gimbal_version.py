#!/usr/bin/env python3

## \file
#  \brief Sync a project's version across its C header, CMakeLists.txt, and Doxyfile.
#
#  Usage:
#       gimbal_version.py [-c version.cfg]                    # read version from source, sync others
#       gimbal_version.py [-c version.cfg] major.minor.patch  # write version to all targets
#
#  version.cfg is looked up in the working directory unless given with -c. Target paths
#  are relative to the directory containing the config file.
#
#       [version]
#       source = header
#
#       [header]
#       path   = include/sh4zam/shz_version.h
#       prefix = SHZ_VERSION
#
#       [cmake]
#       path   = CMakeLists.txt
#       prefix = SHZ_VERSION
#
#       [doxygen]
#       path   = doc/Doxyfile
#
#  Sections may be repeated as "kind:name" (e.g. [cmake:lib]). "source" names the section
#  holding the authoritative version and defaults to the first target.
#
#   \author    2026 Falco Girgis
#   \copyright MIT License

import argparse
import configparser
import re
import sys
from dataclasses import dataclass
from pathlib import Path

CONFIG_NAME = "version.cfg"
COMPONENTS  = ("MAJOR", "MINOR", "PATCH")

Version = tuple[int, int, int]


@dataclass
class Target:
    name:   str
    kind:   str
    path:   Path
    prefix: str | None

    def _component_pattern(self) -> re.Pattern[str]:
        prefix = re.escape(self.prefix)
        if self.kind == "header":
            return re.compile(rf"^(\s*#\s*define\s+{prefix}_(MAJOR|MINOR|PATCH)\s+)(\d+)(.*)$")
        return re.compile(rf"^(\s*set\s*\(\s*{prefix}_(MAJOR|MINOR|PATCH)\s+)(\d+)(\s*\).*)$",
                          re.IGNORECASE)

    def read(self) -> Version:
        lines = self.path.read_text().splitlines()
        if self.kind == "doxygen":
            pattern = re.compile(r"^\s*PROJECT_NUMBER\s*=\s*(.*?)\s*$")
            for line in lines:
                m = pattern.match(line)
                if m:
                    return parse_version(m.group(1).strip('"'))
            raise ValueError(f"PROJECT_NUMBER not found in {self.path}")

        pattern = self._component_pattern()
        fields: dict[str, int] = {}
        for line in lines:
            m = pattern.match(line)
            if m:
                fields[m.group(2).upper()] = int(m.group(3))
        for key in COMPONENTS:
            if key not in fields:
                raise ValueError(f"{self.prefix}_{key} not found in {self.path}")
        return fields["MAJOR"], fields["MINOR"], fields["PATCH"]

    def write(self, version: Version) -> None:
        lines = self.path.read_text().splitlines(keepends=True)
        updated = 0
        if self.kind == "doxygen":
            pattern  = re.compile(r"^(\s*PROJECT_NUMBER\s*=\s*).*$")
            expected = 1
            for i, line in enumerate(lines):
                m = pattern.match(line)
                if m:
                    lines[i] = f"{m.group(1)}{format_version(version)}\n"
                    updated += 1
        else:
            pattern  = self._component_pattern()
            values   = dict(zip(COMPONENTS, version))
            expected = len(COMPONENTS)
            for i, line in enumerate(lines):
                m = pattern.match(line.rstrip("\r\n"))
                if m:
                    lines[i] = f"{m.group(1)}{values[m.group(2).upper()]}{m.group(4)}\n"
                    updated += 1
        if updated != expected:
            raise ValueError(f"Expected {expected} version line(s), found {updated} in {self.path}")
        self.path.write_text("".join(lines))


def parse_version(version_str: str) -> Version:
    parts = version_str.strip().split(".")
    if len(parts) != 3 or not all(p.isdigit() for p in parts):
        raise ValueError(f"Version must be in major.minor.patch format, got: {version_str!r}")
    return int(parts[0]), int(parts[1]), int(parts[2])


def format_version(version: Version) -> str:
    return ".".join(str(v) for v in version)


def load_config(config_path: Path) -> tuple[list[Target], Target]:
    if not config_path.is_file():
        raise FileNotFoundError(f"Config file not found: {config_path}")

    config = configparser.ConfigParser(inline_comment_prefixes=("#", ";"))
    config.read(config_path)
    root = config_path.parent

    targets: list[Target] = []
    for section in config.sections():
        if section == "version":
            continue
        kind = section.split(":", 1)[0]
        if kind not in ("header", "cmake", "doxygen"):
            raise ValueError(f"Unknown target kind [{section}] in {config_path}")
        options = config[section]
        if "path" not in options:
            raise ValueError(f"[{section}] is missing 'path' in {config_path}")
        prefix = options.get("prefix")
        if kind != "doxygen" and not prefix:
            raise ValueError(f"[{section}] is missing 'prefix' in {config_path}")
        targets.append(Target(section, kind, root / options["path"], prefix))

    if not targets:
        raise ValueError(f"No targets defined in {config_path}")

    source_name = config.get("version", "source", fallback=targets[0].name)
    source = next((t for t in targets if t.name == source_name), None)
    if source is None:
        raise ValueError(f"Source [{source_name}] is not a target in {config_path}")

    return targets, source


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("version", nargs="?", metavar="major.minor.patch",
                        help="Version string to write to all targets (default: read from the source target)")
    parser.add_argument("-c", "--config", type=Path, default=Path.cwd() / CONFIG_NAME,
                        help=f"Path to the config file (default: ./{CONFIG_NAME})")
    args = parser.parse_args()

    targets, source = load_config(args.config.resolve())

    if args.version:
        version = parse_version(args.version)
    else:
        version = source.read()
        print(f"Version read from {source.path.name}: {format_version(version)}")

    for target in targets:
        if target is source and not args.version:
            continue
        target.write(version)
        print(f"Updated {target.path.name}")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
