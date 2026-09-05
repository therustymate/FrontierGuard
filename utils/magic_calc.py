import sys


def main(string: str) -> None:
    print(f"{'Index':<8} {'Char':<8} {'Decimal':<10} {'Hex':<10}")
    print("-" * 40)

    for index, char in enumerate(string):
        print(
            f"{index:<8} "
            f"{repr(char):<8} "
            f"{ord(char):<10} "
            f"{f'0x{ord(char):02X}':<10}"
        )


if __name__ == "__main__":
    main(sys.argv[-1])