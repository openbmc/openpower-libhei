#-------------------------------------------------------------------------------
# Chip Data Files
#-------------------------------------------------------------------------------

build_cdb = find_program('parse_chip_data.py')

# The key for each entry in this dictionary is a subdirectory containing XML for
# a chip model. The value for each entry contains the expected output files that
# will be produced for each chip model. It is important to note that the script
# will generate all output files, regardless of what is listed, when the script
# is run. However, this list must be kept in sync with the expected output so
# that meson will know to run the script when an output file has changed or is
# missing.
cdb_files = {}

chip_config = get_option('chip_config')

if 'p10' in chip_config
    cdb_files += {'p10_10' : ['chip_data_p10_10.cdb']}
    cdb_files += {'p10_20' : ['chip_data_p10_20.cdb']}
endif

if 'explorer' in chip_config
    cdb_files += {
        'explorer' : ['chip_data_explorer_11.cdb', 'chip_data_explorer_20.cdb'],
    }
endif

if 'odyssey' in chip_config
    cdb_files += {'odyssey' : ['chip_data_odyssey_10.cdb']}
endif

foreach chip_dir, out_files : cdb_files

    source_dir = meson.current_source_dir() + '/' + chip_dir
    build_dir = meson.current_build_dir()

    # Get all JSON files in the chip directory. This is a bit of a workaround
    # because meson does not allow wildcards.
    json_list = run_command('json_list.sh', source_dir)
    in_files = json_list.stdout().strip().split('\n')

    custom_target(
        'build_cdb_' + chip_dir,
        build_by_default: true,
        input: in_files,
        output: out_files,
        command: [build_cdb, 'bin', source_dir, build_dir],
        install: true,
        install_dir: join_paths(
            get_option('prefix'),
            get_option('datadir'),
            meson.project_name(),
        ),
    )

endforeach

