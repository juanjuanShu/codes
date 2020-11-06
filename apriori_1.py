def load_data_set():
    data_set = [['l1', 'l2', 'l5'], ['l2', 'l4'], ['l2', 'l3'],
                ['l1', 'l2', 'l4'], ['l1', 'l3'], ['l2', 'l3'],
                ['l1', 'l3'], ['l1', 'l2', 'l3', 'l5'], ['l1', 'l2', 'l3']]
    return data_set


def create_C1(data_set):
    C1 = set()

    for t in data_set:
        for item in t:
            item_set = frozenset([item])
            C1.add(item_set)

    return C1


def generate_Lk_by_Ck(data_set, Ck, min_sup_count):
    Lk = set()
    item_count = {}

    for item in Ck:
        for t in data_set:
            if item.issubset(t):
                if item not in item_count:item_count[item] = 1
                else:item_count[item] += 1

    for key,value in item_count.items():
        if value >= min_sup_count:
            Lk.add(key)

    return Lk


def create_Ck(L, k):
    Ck = set()
    L = list(L)
    lenL = len(L)

    for i in range(0,lenL):
        for j in range(i,lenL):
            l1 = list(L[i])
            l2 = list(L[j])
            l1.sort()
            l2.sort()

            if k == 2 and L[i] != L[j]:
                Ck_item = L[i] | L[j]
                Ck.add(Ck_item)
            elif k != 2 and l1[0:k-2] == l2[0:k-2]:
                Ck_item = L[i] | L[j]
                Ck.add(Ck_item)

    return Ck


def generate_L(data_set, min_sup_count):
    C1 = create_C1(data_set)
    L1 = generate_Lk_by_Ck(data_set, C1, min_sup_count)
    L = [0,L1]
    k = 2
    while(len(L[k - 1]) > 0):
        Ck = create_Ck(L[k - 1],k)
        print(Ck)
        exit()
    pass


if __name__ == "__main__":
    data_set = load_data_set()
    # L, support_data = generate_L(data_set, min_sup_count=3)
    generate_L(data_set, min_sup_count=3)
