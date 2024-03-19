import random


def gen_rand_string():
    s = ""
    for i in range(100):
        s += chr(random.randint(ord('a'), ord('z') + 1))
    return s


with open("test.txt", 'r') as file:
    # s = file.readlines()[0]
    # print(s.find('uzkv'))
    file.write(gen_rand_string())
