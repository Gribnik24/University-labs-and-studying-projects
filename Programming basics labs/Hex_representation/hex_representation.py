from num_systems_operations import from_10_into_sth, from_sth_into_10
from functools import lru_cache


def from_2_into_16(num):
    return from_10_into_sth(from_sth_into_10(num, 2), 16)


def from_16_into_2(num):
    return from_10_into_sth(from_sth_into_10(num, 16), 2)


@lru_cache()
def decimal_to_IEEE754(num):
    bit_num = from_10_into_sth(num, 2).lstrip('-')
    dot_finder = bit_num.find('.')
    print(f"bit_num = {bit_num}, exp_len = {len(bit_num)}")

    # Определение знака
    math_sign = '1' if str(num)[0] == '-' else '0'

    # Определение экспоненты
    exp_8_byte = from_10_into_sth(1023 + dot_finder - 1, 2)
    exp_4_byte = from_10_into_sth(127 + dot_finder - 1, 2)
    print(f"exp = {exp_8_byte}, exp_len = {len(exp_8_byte)}")

    # Определение мантиссы (с учетом ограничения по размеру)
    mantis = (bit_num[1:dot_finder] + bit_num[dot_finder + 1:])
    mantis_8_byte = mantis.ljust(52, '0')  # Ограничение для 8-байтного формата
    mantis_4_byte = mantis.ljust(23, '0')  # Ограничение для 4-байтного формата


    print(f"mantis = {mantis_8_byte}, mantis_len = {len(mantis_8_byte)}",
          f'res_8_byte(2) = {math_sign + exp_8_byte + mantis_8_byte}', len(math_sign + exp_8_byte + mantis_8_byte),
          f'res_4_byte(2) = {math_sign} {exp_4_byte} {mantis_4_byte}', sep='\n')


    # Формирование шестнадцатеричного представления
    res_8_byte = from_2_into_16(math_sign + exp_8_byte + mantis_8_byte)
    res_4_byte = from_2_into_16(math_sign + exp_4_byte + mantis_4_byte)

    return res_8_byte, res_4_byte


def IEEE754_to_decimal(num):
    bit_num = from_16_into_2(num)
    if len(num) == 8:
        if len(bit_num) == 31:
            math_sign = ''
            exp = int(from_sth_into_10(bit_num[:8], 2)) - 127
            mantis = from_sth_into_10('1' + bit_num[8:8 + exp] + '.' + bit_num[8 + exp:], 2)
        else:
            math_sign = '-'
            exp = int(from_sth_into_10(bit_num[1:9], 2)) - 127
            mantis = from_sth_into_10('1' + bit_num[9:9 + exp] + '.' + bit_num[9 + exp:], 2)
        return math_sign + mantis

    if len(num) == 16:
        if len(bit_num) == 63:
            math_sign = ''
            exp = int(from_sth_into_10(bit_num[:11], 2)) - 1023
            mantis = from_sth_into_10('1' + bit_num[11:11 + exp] + '.' + bit_num[11 + exp:], 2)
        else:
            math_sign = '-'
            exp = int(from_sth_into_10(bit_num[1:12], 2)) - 1023
            mantis = from_sth_into_10('1' + bit_num[12:12 + exp] + '.' + bit_num[12 + exp:], 2)
        return math_sign + mantis


print(decimal_to_IEEE754(-342.5))

'''
-342.5
3F8A980000000000.24711347123
C3AB4000
'''
'''
13868104976767
13,868,104,976,767,000,000
'''

print('\n', from_sth_into_10(1100000001110101011010000000000000000000000000000000000000000000, 2))
print(from_10_into_sth(1.3868104976767, 16))