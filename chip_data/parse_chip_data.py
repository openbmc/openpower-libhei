#!/usr/bin/env python3

import argparse
import glob
import os

from pyprd.chip_data.binary import binary_encode
from pyprd.chip_data.json import json_decode
from pyprd.chip_data.peltool import peltool_encode


def _import_chip_data(indir: str) -> dict:
    """
    Iterates each JSON file in the given directory (all of which should be
    properly formated Chip Data JSON). Then, combines the data into a singular
    chip_data.Base object for each model/EC.
    """

    bases = dict()

    for file in glob.glob(os.path.join(indir, "*.json")):
        with open(file, "r") as fp:
            for base in json_decode(fp).split():
                # Should be only one model/EC
                assert 1 == len(base.model_ec)
                model_ec = base.model_ec[0]

                if model_ec not in bases:
                    bases[model_ec] = base
                else:
                    bases[model_ec].merge(base)

    return bases


def gen_chip_data_binary(indir: str, outdir: str) -> None:
    for model_ec, base in _import_chip_data(indir).items():
        file = f"chip_data_{model_ec.lower()}.cdb"
        if not os.path.exists(outdir):
            os.makedirs(outdir)
        with open(os.path.join(outdir, file), "wb") as fp:
            binary_encode(model_ec, base, fp)


def gen_peltool_json(cdIndir: str, outdir: str, exSigPath=None) -> None:
    for model_ec, base in _import_chip_data(cdIndir).items():
        file = f"pel_parser_data_{model_ec.lower()}.json"
        if not os.path.exists(outdir):
            os.makedirs(outdir)
        with open(os.path.join(outdir, file), "w") as fp:
            peltool_encode(model_ec, base, fp, exSigPath)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="""
        Parses the target Chip Data files and generates specified data files.
        """
    )

    parser.add_argument(
        "format",
        type=str,
        metavar="<format>",
        choices=["bin", "pel"],
        help="""
        Output data type: 'bin' => Chip Data binary, 'pel' => BMC peltool JSON.
        """,
    )

    parser.add_argument(
        "indir",
        type=str,
        metavar="<input_path>",
        help="""
        Input directory containing the Chip Data files.
        """,
    )

    parser.add_argument(
        "outdir",
        type=str,
        metavar="<output_path>",
        help="""
        Output directory for the generated data.
        """,
    )

    args = parser.parse_args()

    funcs = {
        "bin": "gen_chip_data_binary",
        "pel": "gen_peltool_json",
    }

    globals()[funcs[args.format]](args.indir, args.outdir)
