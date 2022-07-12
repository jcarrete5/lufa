#!/usr/bin/python3
"""
Generates PadConfig.c from PadConfig.c.in with a given pad configuration.
"""
import json
from argparse import ArgumentParser
from pathlib import Path
from typing import Literal

Pad = Literal[
    "RED_PAD",
    "RED_CYM",
    "YEL_PAD",
    "YEL_CYM",
    "BLU_PAD",
    "BLU_CYM",
    "GRN_PAD",
    "GRN_CYM",
    "KICK",
]
PadConfig = dict[Pad, list[int]]


class InvalidConfigValueError(ValueError):
    pass


def parse_config(config_path: Path) -> PadConfig:
    with config_path.open() as file:
        config = json.load(file)

    # Validate configuration
    valid_keys = {
        "RED_PAD",
        "RED_CYM",
        "YEL_PAD",
        "YEL_CYM",
        "BLU_PAD",
        "BLU_CYM",
        "GRN_PAD",
        "GRN_CYM",
        "KICK",
    }
    used = set()
    try:
        for k, v in config.items():
            try:
                valid_keys.remove(k)
            except KeyError:
                raise ValueError(
                    f"invalid pad value specified or specified more than once: {k}"
                )
            for note in v:
                if note in used:
                    raise ValueError("note value already used")
                if note < 0 or note > 127:
                    raise ValueError(
                        "note value must be in the half-open range [0, 128)"
                    )
                used.add(note)
    except Exception:
        raise InvalidConfigValueError

    return config


def generate_pad_config(config: PadConfig):
    mapping = ["X"] * 128
    for k, v in config.items():
        for note in v:
            mapping[note] = k

    with open("src/PadConfig.c.in") as infile, open(
        "./src/PadConfig.c", "w"
    ) as outfile:
        format_str = infile.read()
        outfile.write(format_str.format(mapping=",".join(mapping)))


def main():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument(
        "--config",
        help="Path to pad configuration file",
        type=Path,
        default=Path("./pad_config.json"),
    )
    args = parser.parse_args()
    config = parse_config(args.config)
    generate_pad_config(config)


if __name__ == "__main__":
    main()
