import struct

PuGrYe111 = "#251825,#2B2232,#302D40,#32384E,#31455C,#2E5268,#285F73,#1E6D7D,#107B84,#008989,#01978C,#18A58C,#30B28A,#49BF87,#63CC81,#7FD87B,#9CE373,#BBEE6C,#DBF766,#FDFF63"

PuCy100 = "#251825,#332B3F,#3B415A,#3D5A75,#37748E,#278FA2,#09ABB1,#00C8BA,#30E4BC,#60FFB8"

def generate(hex_colors: str, name = "", min = 0.0, max = 1.0) -> None:
    rgb_colors = (struct.unpack('BBB', bytes.fromhex(x.strip('#'))) for x in hex_colors.split(','))

    code = ("std::make_pair({{:1.2f}}, QColor({:3d}, {:3d}, {:3d}, ColorMapAlpha))".format(c[0], c[1], c[2]) for c in rgb_colors)

    lines = list(code)

    step = max / len(lines)

    print("const QMap<double, QColor> ColorMap_{} = QMap<double, QColor>{{".format(name))
    
    for line in lines[:len(lines)-1]:
        print("  {},".format(line).format(min))
        min += step

    print("  {}".format(lines[len(lines)-1]).format(min))

    print("};")

generate(PuCy100, "PuCy")

generate(PuGrYe111, "PuGrYe")

