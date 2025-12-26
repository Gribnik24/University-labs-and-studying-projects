import math
from typing import Dict, Tuple, List
from parity_codec import add_parity_to_codeword, strip_and_check_parity, augment_codebook_with_parity


class ShannonCoderWithParity:
    def __init__(self):
        """
        Инициализация объектов класса:
        base_codes - коды символов по классическому алгоритму Шеннона
        codes_with_parity - коды символов по алгоритму Шеннона с учетом четности
        base_length - длины кодов символов по классическому алгоритму Шеннона
        """
        self.base_codes: Dict[str, str] = {}
        self.codes_with_parity: Dict[str, str] = {}
        self.base_lengths: Dict[str, int] = {}

    def build_shannon_codes(self, symbols_probs: List[Tuple[str, float]]) -> Dict[str, str]:
        """
        Построение кодов по методу Шеннона
            :param symbols_probs: массив распределния
        """
        sorted_syms = sorted(symbols_probs, key=lambda x: x[1], reverse=True)
        cumulative, codes = 0.0, {}
        for symbol, prob in sorted_syms:
            if prob <= 0:
                continue
            # Длина кода = ceil(-log2(p))
            code_length = math.ceil(-math.log2(prob))
            # Преобразуем кумулятивную вероятность в двоичную дробь
            frac, code = cumulative, []
            for _ in range(code_length):
                frac *= 2
                if frac >= 1:
                    code.append('1'); frac -= 1
                else:
                    code.append('0')
            codes[symbol] = "".join(code)
            # Увеличиваем кумулятивную вероятность после вычисления кода
            cumulative += prob
        self.base_codes = codes
        self.base_lengths = {s: len(c) for s, c in codes.items()}
        self.codes_with_parity = augment_codebook_with_parity(codes)
        return codes

    def encode(self, text: str) -> str:
        """
        Функция по кодированию кода Шеннона
            :param text: текст для кодирования
            :return: закодированный текст с учетом проверки на четность
        """
        parts = []
        for s in text.split():
            base = self.base_codes[s]
            parts.append(add_parity_to_codeword(base))
        return "".join(parts)

    def _build_trie(self) -> dict:
        """
        Строит прфексное дерево на основе кодов символов
        Пример:
        {'A': '00', 'B': '01', 'C': '10'}
        Результирующее дерево будет:
        {
        '0': {'0': {'$': 'A'},
              '1': {'$': 'B'}},
        '1': {'0': {'$': 'C'}}
        }
        """
        trie = {}
        # Проходимся по каждой паре (символ, код с учетом точности)
        for sym, code in self.codes_with_parity.items():
            node = trie
            # Проходим по каждому биту в коде
            for b in code:
                # Создаем вложенные словари для каждого бита
                node = node.setdefault(b, {})
            # В конце пути помечаем символ
            node['$'] = sym

        return trie

    def decode(self, bitstream: str) -> Tuple[str, List[dict]]:
        """
        Выполняет декодирование битового потока с использованием префиксного дерева
        :param bitstream: поток битов
        :return: Массив (декодированный текст, отчет о декодировании)

        Пример отчета о декодировании:
        [{'symbol': 'A', 'start': 0, 'end': 2, 'parity_errors': []},
        {'symbol': 'B', 'start': 2, 'end': 4, 'parity_errors': []},
        {'symbol': 'C', 'start': 4, 'end': 6, 'parity_errors': []}]
        """
        # Строим префиксное дерево из кодов
        trie = self._build_trie()
        i, n, decoded, reports = 0, len(bitstream), [], []
        # Пока не обработали весь поток...
        while i < n:
            # Начинаем с корня дерева
            node, j, sym = trie, i, None
            while j < n and bitstream[j] in node:
                # Переходим вглубь дерева
                node = node[bitstream[j]]; j += 1
                if '$' in node:
                    # Найден символ, запоминаем позицию конца
                    sym = node['$']; end = j

            # когда символ не найден, пропускаем один бит и продолжаем
            if sym is None:
                reports.append({'symbol': None,
                                'start': i,
                                'end': i+1,
                                'parity_errors': ['stream_desync']})
                i += 1
                continue

            # Вырезаем биты кода
            mod_word = bitstream[i:end]
            _, bad = strip_and_check_parity(mod_word, self.base_lengths[sym])
            # Добавляем символ в результат
            decoded.append(sym)
            reports.append({'symbol': sym,
                            'start': i,
                            'end': end,
                            'parity_errors': bad})
            # Перемещаем указатель на конец текущего кода
            i = end
        return " ".join(decoded), reports
