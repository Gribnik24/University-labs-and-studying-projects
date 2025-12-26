from typing import Dict, List, Tuple
"""
Набор функций для работы с четностью в алгоритме Шеннона
"""


def _chunks(bits: str, size: int) -> List[str]:
    """
    Делит поток битов на сегменты
        :param bits: поток битов
        :param size: шаг для деления
        :return: массив, элементы которого - фрагменты входного потока битов
    """
    return [bits[i:i+size] for i in range(0, len(bits), size)]


def _xor_bits(bits: str) -> str:
    """
    Реализация побитового ИЛИ
        :param bits: поток битов
        :return: 1 или 0 в зависимости от реузльтата
    """
    s = 0
    for b in bits:
        s ^= (b == '1')
    return '1' if s else '0'


def add_parity_to_codeword(code: str) -> str:
    """
    Добавляет четность в код
        :param code:
        :return: код с учетом четности
    """
    # Если код пустой, то добавлять нечего
    if not code:
        return code

    # Если len(code) чётная - делим код на группы по 2, иначе - на группы по 3
    block = 2 if len(code) % 2 == 0 else 3

    # Прогоняем каждую группу и добавляем четность по необходимости
    out = []
    for chunk in _chunks(code, block):
        parity = _xor_bits(chunk)
        out.append(chunk + parity)
    return "".join(out)


def strip_and_check_parity(code_with_parity: str, original_len: int) -> Tuple[str, List[int]]:
    """
    Чистит кодовое слово с учетом четности от проверочных битов
        :param code_with_parity: коды с учетом четности
        :param original_len: исходная длина кодового слова
        :return: массив (исходный код, количество найденых проверочных битов)
    """
    # Если код пустой, то выводить нечего
    if original_len == 0:
        return "", []

    # Если len(code) чётная - делим код на группы по 2, иначе - на группы по 3
    block = 2 if original_len % 2 == 0 else 3

    # Поправка +1, т.к. код поступает уже с учетом четности
    mod_block = block + 1

    # Прогонка по фрагментам разбиения
    info_bits, bad, idx = [], [], 1
    for chunk in _chunks(code_with_parity, mod_block):
        # Разделение исходного кода и символа четности
        payload, p = chunk[:-1], chunk[-1]
        # Подсчет и обработка
        if _xor_bits(payload) != p:
            bad.append(idx)
        info_bits.append(payload)
        idx += 1
    return "".join(info_bits)[:original_len], bad


def augment_codebook_with_parity(base_codes: Dict[str, str]) -> Dict[str, str]:
    """
    Добавляет проверочные биты для кодов
        :param base_codes: словарь {символ: исходный коды}
        :return: словарь {символ: новый коды}
    """
    return {sym: add_parity_to_codeword(code) for sym, code in base_codes.items()}
