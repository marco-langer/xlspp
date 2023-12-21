from os import walk
from os.path import splitext
from os.path import join
from os.path import getmtime
from datetime import datetime
import subprocess
import sys
import time

file_extensions = ['.cpp', '.cxx',  '.h', '.hpp']


def find_all_files():
    index: int = -1
    try:
        index = sys.argv.index("--sources")
    except:
        pass

    directories: list[string] = []
    if index >= 0:
        for i in range(index + 1, len(sys.argv)):
            directories.append(sys.argv[i])

    if len(directories) == 0:
        print("source files missing")
        sys.exit(1)

    return find_all_files_in_dir(directories)


def find_all_files_in_dir(directories: list[str]) -> list[str]:
    files: list[string] = []
    for directory in directories:
        for (dirpath, dirnames, filenames) in walk(directory):
            for filename in filenames:
                file_stem, file_extension = splitext(filename)
                if file_extension in file_extensions:
                    files.append(join(dirpath, filename))

    return files


def run_clang_format(files: list[str]) -> int:
    if not files:
        return 0

    if "--dry-run" in sys.argv:
        return subprocess.run(["clang-format","--dry-run", "--Werror"] + files).returncode
    else:
        # TODO this one hour difference between the seconds since epoch and the files last
        # modified time stamp is probably due to wintertime ?!
        time_since_epoch = (datetime.now() - datetime(1970, 1, 1)).total_seconds() - 3600
        result = subprocess.run(["clang-format","-i"] + files)
        if not "--quiet" in sys.argv:
            start = time.time()
            num_files = len(files)
            for i, file in enumerate(files, 1):
                last_modified = getmtime(file)
                was_modified = last_modified > time_since_epoch
                status_label = "up to date"
                if was_modified:
                    status_label = "modified"
                file_index = "({}/{})".format(i, num_files)
                print("{:<8} {:^10} -- {}".format(file_index, status_label, file))
            end = time.time()
            print("completed after: {}s".format(end - start))
        return result.returncode


def main() -> int:
    return run_clang_format(find_all_files())


if __name__ == "__main__":
    sys.exit(main())
