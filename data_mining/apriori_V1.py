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


def generate_Lk_by_Ck(data_set, Ck, min_sup_count, support_data):
    Lk = set()
    item_count = {}

    for item in Ck:
        for t in data_set:
            if item.issubset(t):
                if item not in item_count:
                    item_count[item] = 1
                else:
                    item_count[item] += 1

    for key, value in item_count.items():
        if value >= min_sup_count:
            Lk.add(key)
            support_data[key] = value

    return Lk


def is_apriori(Ck_item, L):
    # k>=3 考虑候选集 k-项集 X={i1，i2,...,ik}。算法必须确定她的所有（k-1）集也是频繁的，即 X - ij
    for item in Ck_item:
        sub_Ck = Ck_item - frozenset([item])
        if sub_Ck not in L:
            return False
    return True


def create_Ck(L, k):
    Ck = set()
    L = list(L)
    lenL = len(L)

    for i in range(0, lenL):
        for j in range(i, lenL):
            l1 = list(L[i])
            l2 = list(L[j])
            l1.sort()
            l2.sort()

            if k == 2 and L[i] != L[j]:

                Ck_item = L[i] | L[j]
                Ck.add(Ck_item)
            elif k != 2 and l1[0:k - 2] == l2[0:k - 2]:
                Ck_item = L[i] | L[j]
                if is_apriori(Ck_item, L):
                    Ck.add(Ck_item)

    return Ck


def generate_L(data_set, min_sup_count):
    # 存储频繁项集合和对应的支持度计数
    support_data = {}

    C1 = create_C1(data_set)
    L1 = generate_Lk_by_Ck(data_set, C1, min_sup_count, support_data)

    L = [0, L1]
    k = 2
    while (len(L[k - 1]) > 0):
        Ck = create_Ck(L[k - 1], k)
        if (len(Ck) > 0):
            Lk = generate_Lk_by_Ck(data_set, Ck, min_sup_count, support_data)
            L.append(Lk)
            k += 1
        else:
            k += 1
            break

    return L[k - 2], support_data


def generate_associate_rules(Lk, support_data, min_conf):
    sub_L = support_data.keys() - Lk

    len_sub_L = len(sub_L)
    len_L = len(Lk)

    sub_L = list(sub_L)
    Lk = list(Lk)

    associate_rules_list = []

    for i in range(0, len_L):
        for j in range(0, len_sub_L):
            if sub_L[j].issubset(Lk[i]):
                # 将 Y分成 x 和 Y - x;P（Y）/P(x)=conf =》关联规则： X=>Y-X conf
                conf = support_data[Lk[i]] / support_data[sub_L[j]]
                if conf >= min_conf:
                    sub_item = Lk[i] - sub_L[j]
                    associate_rule = [sub_L[j], sub_item, conf]
                    associate_rules_list.append(associate_rule)

    return associate_rules_list


def format_conversion(items):
    res_str = ''
    items = list(items)
    items_len = len(items)

    for i in range(0, items_len):
        res_str = res_str + items[i] + '^'
    res_str = res_str.rstrip('^')

    return res_str


def visualization(associate_rules_list):
    len_L = len(associate_rules_list)
    for i in range(0, len_L):
        first_str = format_conversion(associate_rules_list[i][0])
        second_str = format_conversion(associate_rules_list[i][1])
        conf = str(associate_rules_list[i][2] * 100) + "%"
        print(first_str + ' => ' + second_str + ',     ' + 'confidence = ' + conf)


if __name__ == "__main__":
    data_set = load_data_set()
    Lk, support_data = generate_L(data_set, min_sup_count=2)
    associate_rules_list = generate_associate_rules(Lk, support_data, min_conf=0.7)
    visualization(associate_rules_list)
