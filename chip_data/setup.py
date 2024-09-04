import os
import sys

from parse_chip_data import gen_peltool_json
from setuptools import setup
from setuptools.command.build_py import build_py

# Typically in files like this we'd use find_packages() to traverse directories
# for any static packages. However, we are trying to add data to a package that
# will actually exist in another repository. Therefore, we have to explicitly
# list out the package name, directory, and data information.

# We are building data for the following module:
package_name = "pel.hwdiags"

# Since we are not using find_packages() we have to provide a package directory,
# but in this case nothing exists because there are no static package
# directories. Therefore, we will just use the empty string.
package_dir = ""

# Split the package data directory into its components.
data_dir_components = [*package_name.split("."), "data"]

# It is important to note that '/' must be used as the path separator, even on
# Windows. Setuptools will automatically convert the slashes where appropriate.
package_data_glob = "/".join(data_dir_components)

# These are the possible chip config options to build the PEL parser data json
# from. By default all configs will be built. The '--chip-config' option can be
# used in recipes to specify manually.
chipConfig = ["p10_10", "p10_20", "explorer", "odyssey"]


# This is a custom build class that is used to dynamically build the data files.
class my_build_py(build_py):

    def run(self):
        if not self.dry_run:  # honor --dry-run flag
            # Make sure the build directory for the data exists.
            # Yes, os.path.join() is necessary in this case, which is different
            # that what is stated above regarding package_data_glob.
            data_dir = os.path.join(self.build_lib, *data_dir_components)
            self.mkpath(data_dir)

            # Generate the PEL parser data JSON from the Chip Data XML.
            for chip in chipConfig:
                gen_peltool_json(chip, data_dir)

        # Call the superclass run() to ensure everything else builds.
        super().run()


if "--chip-config" in sys.argv:
    chipConfig = sys.argv.pop(sys.argv.index("--chip-config") + 1).split(",")
    sys.argv.remove("--chip-config")

setup(
    name="openpower-hw-diags-pel-parser-data",
    version=os.getenv("PELTOOL_VERSION", "1.0"),
    classifiers=["License :: OSI Approved :: Apache Software License"],
    cmdclass={"build_py": my_build_py},  # register custom build class
    packages=[package_name],
    package_dir={package_name: package_dir},
    package_data={package_name: [package_data_glob]},
)
