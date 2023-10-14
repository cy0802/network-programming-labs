import struct 

def checksum(data, cksum):
    # print(type(data))
    # print(type(data[0]), data[0])
    # print(type(cksum), cksum)
    result = bxor(data[0:2], data[2:4])
    # print(int.from_bytes(data[0:2]))
    # print(int.from_bytes(data[2:4]))
    # print(int.from_bytes(result))
    i = 4
    while i + 2 <= len(data):
        result = bxor(result, data[i:i+2])
        i += 2
    print(cksum, int.from_bytes(result))
    if cksum == int.from_bytes(result):
        # print("true")
        return True
    else:
        # print("false")
        return False

def bxor(ba1, ba2):
    """ XOR two byte strings """
    return bytes([_a ^ _b for _a, _b in zip(ba1, ba2)])

with open("demo1.bin", 'rb') as f:
    # read header
    data = f.read(16)
    magic, datasize, n_blocks, zeros = struct.unpack('>8sIHH', data)
    print(f"header info\n\ndatasize: {datasize}\nblocks: {n_blocks}")
    print("\n===========================================================")
    dictionary = []
    # process each block
    print("block info\n")
    for i in range(n_blocks):
        data = f.read(8)
        offset, cksum, length = struct.unpack('>IHH', data)
        print(f"offset: {offset}, cksum: {cksum}, length: {length}")
        data = f.read(length)
        if checksum(data, cksum):
            dictionary.append((offset, struct.unpack(f'>{length}s', data)))
            print("good")
        else:
            print("corrupt")
        print(dictionary)
        print("\n-------------------------------------------------------------")
        
    dictionary.sort()
    dic = dictionary[0][1][0]
    # print(type(dic))
    print("block result\n")
    print(f"dictionary after sorting\n{dictionary}")
    for i in range(1, len(dictionary)):
        tmp = dictionary[i][1][0]
        dic = dic + tmp
    print(dic)
    # print(str(dic[64]))
    print("======================================================================\n")
    # print(dictionary)
    # print()
    # for i in len(dictionary):

    # read flag info
    data = f.read(2)
    data = struct.unpack('>H', data)
    length = data[0]
    # print(length)
    print("flag info")
    for i in range(length):
        data = f.read(4)
        data = struct.unpack('>I', data)
        print(data, chr(dic[data[0]]), chr(dic[data[0]+1]))


# print(a, b, c)
# print(type(a), type(b), type(c))