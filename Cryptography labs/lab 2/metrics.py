import math


def average_code_length(codes, prob_dict):
    """Вычисляет среднюю длину кодового слова
        :param codes: массив символов
        :param prob_dict: словарь формата {символ: вреоятность}
        """
    return sum(prob_dict[sym] * len(codes[sym]) for sym in codes if sym in prob_dict)


def entropy(prob_dict):
    """Вычисляет энтропию источника
        :param prob_dict: словарь формата {символ: вреоятность}
        """
    return -sum(prob * math.log2(prob) for prob in prob_dict.values() if prob > 0)


def kraft_inequality(codes):
    """Сборка суммы для неравенства Крафта
    :param codes: массив символов
    """
    return sum(2 ** -len(code) for code in codes.values())


def calculate_metrics(codes, prob_dict):
    """Вычисляет все метрики
    :param codes: массив символов
    :param prob_dict: словарь формата {символ: вреоятность}
    """
    avg_len = average_code_length(codes, prob_dict)  # Средняя длина кодового слова
    entropy_val = entropy(prob_dict)  # Энтропия
    redundancy_val = avg_len - entropy_val  # Избыточность
    kraft_sum = kraft_inequality(codes)  # Сумма для неравенства Крафта
    kraft_check = kraft_sum <= 1  # Неравенство Крафта

    return {
        'average_length': round(avg_len, 4),
        'entropy': round(entropy_val, 4),
        'redundancy': round(redundancy_val, 4),
        'kraft_inequality': kraft_check,
        'kraft_sum': round(kraft_sum, 4)
    }
