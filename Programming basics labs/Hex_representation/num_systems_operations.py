from decimal import Decimal

float_part_accuracy = 15


def alph_nums_trans(num, to_decimal=True):
    alph = {
        '1': '1', '2': '2', '3': '3', '4': '4', '5': '5',
        '6': '6', '7': '7', '8': '8', '9': '9', 'A': '10',
        'B': '11', 'C': '12', 'D': '13', 'E': '14', 'F': '15',
        '0': '0', '.': '.'
    }
    res = ''
    if to_decimal:
        for symbol in str(num):
            res += alph[symbol]
    else:
        res = list(alph.keys())[list(alph.values()).index(str(num))]
    return res


def edges_zero_adding(a, b):
    max_float_len = max(len(a[a.find('.') + 1:]) if '.' in a else 0, len(b[b.find('.') + 1:]) if '.' in b else 0)
    max_int_len = max(len(a[:a.find('.')]) if '.' in a else len(a), len(b[:b.find('.')]) if '.' in b else len(b))

    if '.' in a:
        new_a = '0' * (max_int_len - len(a[:a.find('.')])) + a + '0' * (max_float_len - len(a[a.find('.') + 1:]))
    else:
        new_a = '0' * (max_int_len - len(a)) + a
    if '.' in b:
        new_b = '0' * (max_int_len - len(b[:b.find('.')])) + b + '0' * (max_float_len - len(b[b.find('.') + 1:]))
    else:
        new_b = '0' * (max_int_len - len(b)) + b
    return new_a, new_b


def from_10_into_sth(number, new_system: int):
    if '.' in str(number):
        int_part = int(abs(float(number)))
        float_part = Decimal(str(number).lstrip('-')) - int_part
        new_number = '.'
        float_count = 0
        while float_part != 0.0 and float_count < float_part_accuracy:
            float_part *= new_system
            new_number += alph_nums_trans(str(int(float_part)), False)
            float_part -= int(float_part)
            float_count += 1
    else:
        int_part, new_number = number, ''
    if int_part == 0:
        return '0' + new_number
    while int_part != 0:
        new_number = alph_nums_trans(int_part % new_system, False) + new_number
        int_part //= new_system
    if '.' in str(new_number):
        new_number = str(new_number).rstrip('0').rstrip('.')
    if str(number)[0] == '-':
        new_number = '-' + new_number
    return new_number


def from_sth_into_10(number, old_system: int):
    new_number, old_number = 0, str(number).lstrip('-')
    if '.' in old_number:
        int_symbols_count = len(old_number[:old_number.find('.')]) - 1
    else:
        int_symbols_count = len(old_number) - 1
    for num in old_number:
        if num == '.':
            pass
        else:
            new_number += Decimal(alph_nums_trans(num)) * Decimal(pow(old_system, int_symbols_count))
            int_symbols_count -= 1
    '''    
    if len(str(new_number)) > 14:
        new_number = str(round(float(new_number), 14))
        print(type(new_number))
    '''
    if '.' in str(new_number):
        new_number = str(new_number).rstrip('0').rstrip('.')
    if str(number)[0] == '-':
        new_number = '-' + new_number
    return new_number


def unusual_nums_sum(num1, num2, system: int):
    a, b = map(str, (num1, num2))
    a, b = map(lambda x: x[::-1], edges_zero_adding(a, b))
    res = ''
    div_remainder = 0
    for num in range(len(a)):
        if a[num] == '.':
            res += '.'
        else:
            digit_sm = int(alph_nums_trans(a[num])) + int(alph_nums_trans(b[num]))
            res += alph_nums_trans(str((digit_sm + div_remainder) % system), False)
            div_remainder = (digit_sm + div_remainder) // system
    res += str(div_remainder)
    if '.' in res:
        return res[::-1].strip('0')
    return res[::-1].lstrip('0')


def unusual_nums_sub(num1, num2, system):
    a, b = map(str, (num1, num2))
    a, b = map(lambda x: x[::-1], edges_zero_adding(a, b))

    res = ''
    sub_remainder = 0
    for num in range(len(a)):
        if a[num] == '.':
            res += '.'
        else:
            digit_sub = int(alph_nums_trans(a[num])) - int(alph_nums_trans(b[num])) - sub_remainder
            if digit_sub >= 0:
                res += alph_nums_trans(str(digit_sub), False)
                sub_remainder = 0
            else:
                res += alph_nums_trans(str(digit_sub + system), False)
                sub_remainder = 1

    if '.' in res:
        return res[::-1].strip('0')
    return res[::-1].lstrip('0')


def unusual_nums_mult(num1, num2, system):
    float_part_length, res = 0, 0
    a, b = map(str, (num1, num2))
    if '.' in a and '.' in b:
        a_dot = a.find('.')
        b_dot = b.find('.')
        float_part_length = len(a[a_dot + 1:]) + len(b[b_dot + 1:])
        a, b = a[:a_dot] + a[a_dot + 1:], b[:b_dot] + b[b_dot + 1:]
    elif '.' in a and '.' not in b:
        a_dot = a.find('.')
        float_part_length = len(a[a_dot + 1:])
        a = a[:a_dot] + a[a_dot + 1:]
    elif '.' not in a and '.' in b:
        b_dot = b.find('.')
        float_part_length = len(b[b_dot + 1:])
        b = b[:b_dot] + b[b_dot + 1:]

    sm_container = []
    end_zero_num = 0
    for num in b[::-1]:
        if num == '0':
            end_zero_num += 1
            continue
        mid_res = a
        for _ in range(int(alph_nums_trans(num)) - 1):
            mid_res = unusual_nums_sum(mid_res, a, system)
        sm_container.append(mid_res + '0' * end_zero_num)
        end_zero_num += 1
    for elem in sm_container:
        res = unusual_nums_sum(res, elem, system)

    if float_part_length:
        return (res[:len(res) - float_part_length] + '.' + res[len(res) - float_part_length:]).rstrip('0').rstrip('.')
    return res
