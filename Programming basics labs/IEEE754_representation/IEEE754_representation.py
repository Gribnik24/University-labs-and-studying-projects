# Импорт функций перевода сс из прошлой работы и lru_cache для кэширования вызовов функций (не помешает для тестов)
from num_systems_operations import from_10_into_sth, from_sth_into_10
from functools import lru_cache


def from_2_into_16(num):
    return from_10_into_sth(from_sth_into_10(num, 2), 16)


def from_16_into_2(num):
    return from_10_into_sth(from_sth_into_10(num, 16), 2)


def mantis_round(old_mantis):
    """
        Функция для округления мантиссы в соответствии с правилами IEEE-754.

        Args:
            old_mantis (str): Двоичное представление мантиссы.

        Returns:
            str: Округленная двоичная мантисса.
    """
    new_mantis = old_mantis
    if new_mantis[-1] == '0':
        return new_mantis
    for sym in range(len(new_mantis) - 2, 0, -1):
        if new_mantis[sym] == '0':
            new_mantis = new_mantis[:sym] + '1' + new_mantis[sym + 1: -2] + '0'
            return new_mantis


@lru_cache()  # Декоратор для кэширования
def decimal_to_ieee754(num):
    """
        Функция для преобразования десятичного числа в представление IEEE-754.

        Args:
            num (float | str): Десятичное число.

        Returns:
            tuple: Кортеж, содержащий шестнадцатеричные представления числа в 8-байтном
                   и 4-байтном форматах IEEE-754.
    """
    bit_num = from_10_into_sth(num, 2, float_part_accuracy=58).lstrip('-')
    dot_finder = bit_num.find('.')

    # Определение знака
    math_sign = '1' if str(num)[0] == '-' else '0'

    if bit_num[0] != '0':
        # Если число имеет целую часть, то сдвиг по разрядам для мантиссы численно равен индексу точки
        zero_count = dot_finder

        # Определение мантиссы (с учетом ограничения по размеру)
        # ljust для донабора кол-ва разрядов нулями и срез для отсечения лишнего, если число больше
        mantis = (bit_num[1:dot_finder] + bit_num[dot_finder + 1:])
        mantis_8_byte = mantis.ljust(52, '0')[:52]  # Ограничение для 8-байтного формата
        mantis_4_byte = mantis.ljust(23, '0')[:23]  # Ограничение для 4-байтного формата

        # Определение экспоненты
        exp_8_byte = from_10_into_sth(1023 + zero_count - 1, 2)
        exp_4_byte = from_10_into_sth(127 + zero_count - 1, 2)

    else:
        # Определение мантиссы (с учетом ограничения по размеру)
        # lstrip обрежет нули слева от числа - нулевую целую часть
        # ф-ия mantis_round округлит мантиссу по правилам IEEE-754 в случае необходимости
        mantis = (bit_num[1:dot_finder] + bit_num[dot_finder + 1:]).lstrip('0')
        mantis_8_byte = mantis_round(mantis[1:54])[:52]  # Ограничение для 8-байтного формата
        mantis_4_byte = mantis_round(mantis[1:25])[:23]  # Ограничение для 4-байтного формата

        # Здесь сдвиг по разрядам - определим по формуле:
        # Длина двоичного представления входного числа - длина мантиссы без преобразований - 1
        zero_count = len(bit_num) - len(mantis) - 1

        # Определение экспоненты и донабор знаков до нужной длины справа с помощью rjust
        exp_8_byte = from_10_into_sth(1023 - zero_count, 2).rjust(11, '0')
        exp_4_byte = from_10_into_sth(127 - zero_count, 2).rjust(8, '0')

    # Вывод всех промежуточных данных (для тестов и проверки)
    '''
    print('-----------------------')
    print(f"bit_num = {bit_num}, bit_num_len = {len(bit_num)}") # Вывод двоичного представления заданного числа
    print(f"exp4 = {exp_4_byte}, exp4_len = {len(exp_4_byte)}")
    print(f"exp8 = {exp_8_byte}, exp8_len = {len(exp_8_byte)}")
    print(f'zero_count = {zero_count}')
    print(f"mantis4 = {mantis_4_byte}, mantis_len = {len(mantis_4_byte)}",
          f"mantis8 = {mantis_8_byte}, mantis_len = {len(mantis_8_byte)}",
          f'res_8_byte_whitespace(2) = {math_sign} {exp_8_byte} {mantis_8_byte}',
          f'res_4_byte_whitespace(2) = {math_sign} {exp_4_byte} {mantis_4_byte}',
          f'res_8_byte(2) =',
          f'{math_sign + exp_8_byte + mantis_8_byte}',
          f'res_4_byte(2) =',
          f'{math_sign + exp_4_byte + mantis_4_byte}', sep='\n')
    print('-----------------------')
    '''

    # Формирование шестнадцатеричного представления конечного вывода
    res_8_byte = from_2_into_16(math_sign + exp_8_byte + mantis_8_byte)
    res_4_byte = from_2_into_16(math_sign + exp_4_byte + mantis_4_byte)
    return res_8_byte, res_4_byte


# Перевод из стандарта IEEE-754 в дробный вид
def ieee754_to_decimal(num):
    """
        Функция для преобразования представления IEEE-754 в десятичное число.

        Args:
            num (str): Число в 4-байтном или 8-байтном представлении IEEE-754.

        Returns:
            str: Знак и значения десятичного числа.
    """
    bit_num = from_16_into_2(num) # Получение двочиного представления числа
    if len(num) == 8: # Если кол-во символов ввода ранво 8, то это 4-байтный вид
        if len(bit_num) == 31: # Если кол-во символов в 2 сс равно 31, то первое число 0. Ответ будет положительным
            math_sign = ''
            exp = int(from_sth_into_10(bit_num[:8], 2)) - 127 # Находим экспоненту
            mantis = from_sth_into_10('1' + bit_num[8:8 + exp] + '.' + bit_num[8 + exp:], 2) #...и мантиссу
        else:
            math_sign = '-' # ...иначе число знаков - 32, первый знак - '1'
            exp = int(from_sth_into_10(bit_num[1:9], 2)) - 127
            mantis = from_sth_into_10('1' + bit_num[9:9 + exp] + '.' + bit_num[9 + exp:], 2)
        return math_sign + mantis

    if len(num) == 16: # Если кол-во символов ввода ранво 16, то это 8-байтный вид
        if len(bit_num) == 63:
            math_sign = ''
            exp = int(from_sth_into_10(bit_num[:11], 2)) - 1023
            mantis = from_sth_into_10('1' + bit_num[11:11 + exp] + '.' + bit_num[11 + exp:], 2)
        else:
            math_sign = '-'
            exp = int(from_sth_into_10(bit_num[1:12], 2)) - 1023
            mantis = from_sth_into_10('1' + bit_num[12:12 + exp] + '.' + bit_num[12 + exp:], 2)
        return math_sign + mantis
