# Вариант 2: Шифр Бофора (Beaufort) над алфавитом a..z и '_' (разделитель слов)
# Формулы (|X|=27):
#   шифрование:   c = (k - m) mod 27
#   дешифрование: m = (k - c) mod 27
# Где m, c, k — индексы символов в алфавите

from typing import Dict

# Алфавит из 27 символов: 26 букв + символ '_' для разделителя слов
ALPHABET = "abcdefghijklmnopqrstuvwxyz_"
# Длина алфавита
_N = len(ALPHABET)
# Словарь для быстрого поиска индекса символа в алфавите
_IDX: Dict[str, int] = {ch: i for i, ch in enumerate(ALPHABET)}


def normalize_text(s: str) -> str:
    """
    Приводим текст к требованиям задания:
    - нижний регистр
    - любые пробелы/переводы строк/табуляции -> одинарный '_' (разделитель слов)
    - прочие символы остаются как есть (если не в алфавите — не шифруем и не тратим ключ)
    """
    # Приводим все символы к нижнему регистру
    s = s.lower()
    # Заменяем все виды пробельных символов на обычные пробелы
    s = s.replace("\t", " ").replace("\r", " ").replace("\n", " ")
    # Схлопываем множественные пробелы и заменяем на '_'
    s = "_".join(" ".join(s.split()).split(" "))
    return s


def _repeat_key_indices(key: str, symbols_to_cover: int):
    """
    Генератор для циклического повторения ключа.
    Ключ нормализуем, фильтруем по алфавиту и повторяем циклически только
    для позиций, которые действительно шифруются (символ в ALPHABET).

    Args:
        key: исходный ключ
        symbols_to_cover: количество символов, которые нужно зашифровать
    """
    # Нормализуем ключ (приводим к нижнему регистру и заменяем пробелы на '_')
    key = normalize_text(key)
    # Оставляем только символы из алфавита
    key = "".join(ch for ch in key if ch in ALPHABET)
    if not key:
        raise ValueError("Ключ пустой: после нормализации не осталось символов a..z или '_'")
    # Преобразуем символы ключа в их числовые индексы
    nums = [_IDX[ch] for ch in key]
    i = 0
    # Генерируем индексы ключа пока не покроем все нужные символы
    while i < symbols_to_cover:
        for v in nums:
            if i >= symbols_to_cover:
                break
            yield v
            i += 1


def encrypt(plaintext: str, key: str) -> str:
    """Шифрование Бофора: c = (k - m) mod N. Неалфавитные символы не шифруются и ключ не расходуют."""
    if key is None:
        raise ValueError("Не задан ключ шифрования.")
    # Нормализуем исходный текст
    pt = normalize_text(plaintext)
    out = []
    # Подсчитываем сколько символов нужно зашифровать (только те, что в алфавите)
    # и создаем генератор ключа на это количество
    key_iter = _repeat_key_indices(key, sum(1 for ch in pt if ch in ALPHABET))
    for ch in pt:
        if ch in ALPHABET:
            # Берем следующий символ ключа
            k = next(key_iter)
            # Получаем индекс символа открытого текста
            m = _IDX[ch]
            # Применяем формулу шифрования Бофора: c = (k - m) mod N
            c = (k - m) % _N
            # Преобразуем числовой результат обратно в символ
            out.append(ALPHABET[c])
        else:
            # Неалфавитные символы добавляем как есть
            out.append(ch)
    return "".join(out)


def decrypt(ciphertext: str, key: str) -> str:
    """Дешифрование Бофора: m = (k - c) mod N. Неалфавитные символы не шифруются и ключ не расходуют."""
    if key is None:
        raise ValueError("Не задан ключ для дешифрования.")
    # Нормализуем зашифрованный текст
    ct = normalize_text(ciphertext)
    out = []
    # Подсчитываем сколько символов нужно расшифровать (только те, что в алфавите)
    # и создаем генератор ключа на это количество
    key_iter = _repeat_key_indices(key, sum(1 for ch in ct if ch in ALPHABET))
    for ch in ct:
        if ch in ALPHABET:
            # Берем следующий символ ключа
            k = next(key_iter)
            # Получаем индекс символа шифротекста
            c = _IDX[ch]
            # Применяем формулу дешифрования Бофора: m = (k - c) mod N
            m = (k - c) % _N
            # Преобразуем числовой результат обратно в символ
            out.append(ALPHABET[m])
        else:
            # Неалфавитные символы добавляем как есть
            out.append(ch)
    return "".join(out)