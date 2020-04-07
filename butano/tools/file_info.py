"""
Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
MIT License, see LICENSE file.
"""

import os
import string

class FileInfo:

    @staticmethod
    def validate(file_name):
        valid_characters = '-_.%s%s' % (string.ascii_letters, string.digits)

        for file_name_character in file_name:
            if file_name_character not in valid_characters:
                raise ValueError('Invalid file name: ' + file_name)

    @staticmethod
    def read(file_path):
        info = ''
        read_failed = True

        if os.path.isfile(file_path):
            with open(file_path, 'r') as file:
                info = file.read()
                read_failed = False

        return FileInfo(info, read_failed)

    @staticmethod
    def build_from_file(file_path):
        return FileInfo.build_from_files([file_path])

    @staticmethod
    def build_from_files(file_paths):
        info = []

        for file_path in file_paths:
            info.append(file_path)
            info.append(str(os.path.getsize(file_path)))
            info.append(str(os.path.getmtime(file_path)))

        return FileInfo('\n'.join(info), False)

    def __init__(self, info, read_failed):
        self.__info = info
        self.__read_failed = read_failed

    def write(self, file_path):
        with open(file_path, 'w') as file:
            file.write(self.__info)

    def __eq__(self, other):
        return self.__info == other.__info and self.__read_failed == other.__read_failed

    def __ne__(self, other):
        return self.__info != other.__info or self.__read_failed != other.__read_failed
