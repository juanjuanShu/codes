def load_data_set():
    data_set = [['l1', 'l2', 'l5'], ['l2', 'l4'], ['l2', 'l3'],
                ['l1', 'l2', 'l4'], ['l1', 'l3'], ['l2', 'l3'],
                ['l1', 'l3'], ['l1', 'l2', 'l3', 'l5'], ['l1', 'l2', 'l3']]
    return data_set


def create_C1(data_set):
    temp1 = set()
    C1 = {}

    for t in data_set:
        for item in t:
            temp1.add(item)

    list1 = list(temp1)
    #按照字典序排序
    list1.sort()
    C1 = C1.fromkeys(list1,0)
    for t in data_set:
        for item in t:
            C1[item] += 1

    return C1


def generate_Lk_by_Ck(C1,min_sup_count):
    print(C1)
    for key,value in C1.items():
        print(value)
        if value < min_sup_count:
            del C1[key]
    return C1


def my_apriori(data_set, min_sup_count):
    # 找到L1
    C1 = create_C1(data_set)
    L1 = generate_Lk_by_Ck(C1,min_sup_count)
    print(L1)
    #support_data 支持度计数数组还是要保留的





if __name__ == "__main__":
    data_set = load_data_set()
    min_sup_count = 3
    # 调用Apriori算法产生频繁项集
    my_apriori(data_set, min_sup_count)
