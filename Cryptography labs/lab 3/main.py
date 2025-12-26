import streamlit as st
import pandas as pd

from ShannonCoder_WithParity import ShannonCoderWithParity
from HammingCoder_74 import HammingCoder74
from metrics import calculate_metrics, calculate_hamming_metrics, generate_syndrome_table, calculate_shannon_parity_metrics, calculate_shannon_bounds


def create_distribution(alphabet_file, odds_file):
    """
    Функция по созданию массива распределения
        :param alphabet_file: txt файл с алфавитом символов
        :param odds_file: txt файл с вероятностями
        :return: массив из пар элементов (символ, вероятность)
    """
    distribution_array = []
    with open(alphabet_file) as f:
        alphabet = f.read().split()
    with open(odds_file) as f:
        odds = f.read().split()
    for symbol, odd in zip(alphabet, odds):
        distribution_array.append((symbol, eval(odd)))
    return distribution_array


def main():
    """
    Функция по выводу и оформлению основной web-страницы
    """
    distribution = create_distribution('alphabet.txt', 'uniform odds.txt')

    st.set_page_config(page_title="Кодирование: Шеннон + Хэмминг", layout="wide")
    st.title("Лаба 3 — Шеннон (с чётностью) и Хэмминг (7,4)")

    st.sidebar.title("Навигация")
    family = st.sidebar.radio("Выберите код:", ["Шеннон + чётность", "Хэмминг (7,4)"])
    mode = st.sidebar.radio("Режим:", ["Кодирование", "Декодирование"])

    if family == "Шеннон + чётность":
        if mode == "Кодирование":
            render_shannon_encoding(distribution)
        else:
            render_shannon_decoding(distribution)
    else:
        if mode == "Кодирование":
            render_hamming_encoding()
        else:
            render_hamming_decoding()

# ---------- ШЕННОН ----------
def render_shannon_encoding(distribution):
    """
    Функция со страницей кодирования через алгоритм Шеннона
        :param distribution: словарь с возможными распределениями
    """
    st.header("Часть I: Шеннон + проверка на чётность")

    col1, col2 = st.columns(2)
    with col1:
        symbols_probs = distribution
        df = pd.DataFrame(symbols_probs, columns=["Символ", "Вероятность"])
        st.dataframe(df, use_container_width=True)

        # Добавляем загрузку файла для Шеннона
        up = st.file_uploader("Файл с символами (через пробел/перевод строки):", type=["txt"], key="shannon_upload")
        if up:
            file_content = up.read().decode("utf-8").strip()
            default_text = " ".join(file_content.split())
        else:
            default_text = "2 20 0 200 00 _"

        text = st.text_area("Текст (символы через пробел):", value=default_text, height=100)

    with col2:
        if st.button("Кодировать (Шеннон + чётность)", type="primary"):
            try:
                coder = ShannonCoderWithParity()
                base_codes = coder.build_shannon_codes(symbols_probs)
                encoded = coder.encode(text)
                prob_dict = dict(symbols_probs)
                metrics = calculate_metrics(base_codes, prob_dict)

                # Рассчитываем метрики для кода Шеннона с четностью
                shannon_parity_metrics = calculate_shannon_parity_metrics(base_codes, coder.codes_with_parity)
                # Рассчитываем границы для Шеннона
                shannon_bounds = calculate_shannon_bounds(shannon_parity_metrics)

                st.success("Готово!")

                # Основные метрики Шеннона
                st.subheader("Кодовые слова (Шеннон)")
                st.dataframe(pd.DataFrame(
                    [{"Символ": s, "Код": c, "Длина": len(c)} for s, c in base_codes.items()]
                ), use_container_width=True)

                st.subheader("Кодовые слова (с чётностью)")
                st.dataframe(pd.DataFrame(
                    [{"Символ": s, "Код": coder.codes_with_parity[s], "Длина": len(coder.codes_with_parity[s])}
                     for s in base_codes.keys()]
                ), use_container_width=True)

                # Все метрики Шеннона в табличном виде
                st.subheader("Метрики эффективности кодирования")
                eff1, eff2 = st.columns(2)
                with eff1:
                    st.metric("Средняя длина", f"{metrics['average_length']:.4f}")
                    st.metric("Энтропия", f"{metrics['entropy']:.4f}")
                    st.metric("Избыточность", f"{metrics['redundancy']:.4f}")
                with eff2:
                    st.metric("Неравенство Крафта", "Выполнено" if metrics['kraft_inequality'] else "Не выполнено")
                    st.metric("Сумма Крафта", f"{metrics['kraft_sum']:.4f}")
                    st.metric("Эффективность", f"{metrics['entropy']/metrics['average_length']:.4f}")

                # Метрики помехоустойчивости Шеннона
                st.subheader("Метрики помехоустойчивости (Шеннон + чётность)")
                p1, p2, p3 = st.columns(3)
                with p1:
                    st.metric("Кодовое расстояние", shannon_parity_metrics['code_distance'])
                    st.metric("Средняя длина с чётностью", f"{shannon_parity_metrics['average_length']:.4f}")
                with p2:
                    st.metric("Обнаруживает ошибок", shannon_parity_metrics['detectable_errors'])
                    st.metric("Исправляет ошибок", shannon_parity_metrics['correctable_errors'])
                with p3:
                    st.metric("Минимальная длина", shannon_parity_metrics['min_length'])
                    st.metric("Максимальная длина", shannon_parity_metrics['max_length'])

                # Теоретические границы для Шеннона
                st.subheader("Теоретические границы (для кода с чётностью)")
                bounds1, bounds2, bounds3 = st.columns(3)
                with bounds1:
                    st.metric("Граница Хэмминга", "Выполнена" if shannon_bounds['hamming_bound'] else "Не выполнена")
                with bounds2:
                    st.metric("Граница Плоткина", str(shannon_bounds['plotkin_bound']))
                with bounds3:
                    st.metric("Граница Варшамова-Гильберта", shannon_bounds['varshamov_bound'])

                # Информация о возможностях кода
                detection_status = "Обнаруживает ошибки" if shannon_parity_metrics['is_error_detecting'] else "Не обнаруживает ошибки"
                correction_status = "Исправляет ошибки" if shannon_parity_metrics['is_error_correcting'] else "Не исправляет ошибки"

                st.info(f"**Возможности кода:** {detection_status} | {correction_status}")
                st.info(f"**Размер алфавита:** {shannon_parity_metrics['number_of_codewords']} кодовых слов")

                # Результат кодирования
                st.subheader("Результат кодирования")
                st.write(f"Исходный текст: {text}")
                st.write(f"Общая длина: {len(encoded)} бит")

                # Форматируем вывод для лучшей читаемости
                st.text_area("Закодированная бинарная последовательность:", encoded, height=120)

                st.download_button("Скачать закодированные данные (TXT)",
                                   data=encoded, file_name="shannon_parity_encoded.txt", mime="text/plain")

            except Exception as e:
                st.error(str(e))


def render_shannon_decoding(distribution):
    """
    Функция со страницей декодирования через алгоритм Шеннона
        :param distribution: словарь с возможными распределениями
    """
    st.header("Часть I: Декодирование (Шеннон + чётность)")

    col1, col2 = st.columns(2)
    with col1:
        symbols_probs = distribution
        df = pd.DataFrame(symbols_probs, columns=["Символ", "Вероятность"])
        st.dataframe(df, use_container_width=True)

        # Добавляем загрузку файла для декодирования Шеннона
        up = st.file_uploader("Файл с бинарной последовательностью:", type=["txt"], key="shannon_decode_upload")
        if up:
            file_content = up.read().decode("utf-8").strip()
            default_bits = file_content
        else:
            default_bits = ""

        bits = st.text_area("Бинарная последовательность:", value=default_bits, height=120, placeholder="0/1...")

    with col2:
        if st.button("Декодировать", type="primary"):
            try:
                coder = ShannonCoderWithParity()
                coder.build_shannon_codes(symbols_probs)
                decoded, reports = coder.decode(bits)

                st.success("Готово!")

                # Статистика декодирования
                total_symbols = len(reports)
                error_blocks = sum(1 for r in reports if r['parity_errors'])
                successful_symbols = total_symbols - len([r for r in reports if r['parity_errors']])

                st.subheader("Статистика декодирования")
                stat1, stat2, stat3 = st.columns(3)
                with stat1:
                    st.metric("Всего символов", total_symbols)
                with stat2:
                    st.metric("Успешно декодировано", successful_symbols)
                with stat3:
                    st.metric("Блоков с ошибками", error_blocks)

                st.text_area("Декодированный текст:", decoded, height=100)

                st.subheader("Детальный отчёт о проверке чётности")
                df = pd.DataFrame(reports)
                st.dataframe(df, use_container_width=True)

                st.download_button("Скачать декодированный текст (TXT)",
                                   data=decoded, file_name="shannon_parity_decoded.txt", mime="text/plain")

                # Отчёт об ошибках
                err_only = [r for r in reports if r['parity_errors']]
                err_txt = "\n".join([f"[{r['start']}:{r['end']}) {r['symbol']} -> ошибки в блоках {r['parity_errors']}" for r in err_only]) \
                          or "Ошибок не обнаружено"
                st.download_button("Скачать отчёт об ошибках (TXT)",
                                   data=err_txt, file_name="shannon_parity_errors.txt", mime="text/plain")
            except Exception as e:
                st.error(str(e))


# ---------- ХЭММИНГ ----------
def _bin4(x: int) -> str:
    """
    Перевод числа в двоичную систему
    :param x: Число для перевода
    :return: Число в двоичной системе, дополненное нулями до длины 4
    """
    if not (0 <= x <= 15):
        raise ValueError("Символ вне диапазона 0..15")
    return format(x, "04b")


def render_hamming_encoding():
    """
    Функция со страницей кодирования через алгоритм Хэмминга
    """
    st.header("Часть II: Кодирование по Хэммингу (7,4) — вариант 2, заданный G")

    col1, col2 = st.columns(2)
    with col1:
        up = st.file_uploader("Файл с числами 0..15 (через пробел/перевод строки):", type=["txt"], key="hamming_upload")
        default = "1 2 3 4 5 6 7" if not up else " ".join(up.read().decode("utf-8").split())
        text = st.text_area("Последовательность чисел 0..15:", value=default, height=120)

    with col2:
        if st.button("Кодировать (Хэмминг 7,4)", type="primary"):
            try:
                seq = [int(x) for x in text.split()]
                coder = HammingCoder74()
                encoded = coder.encode(seq)

                # Рассчитываем метрики Хэмминга
                hamming_metrics = calculate_hamming_metrics()
                syndrome_table = generate_syndrome_table()

                st.success("Готово!")

                # Результат кодирования
                st.subheader("Результат кодирования")
                st.write(f"Исходные числа: {seq}")
                st.write(f"Количество символов: {len(seq)}")
                st.write(f"Общая длина: {len(encoded)} бит ({len(seq)} × 7 бит)")

                # Форматируем с пробелами между словами
                formatted_bits = ' '.join([encoded[i:i+7] for i in range(0, len(encoded), 7)])
                st.text_area("Закодированная последовательность (разделена по 7 бит):",
                             formatted_bits, height=100)

                # Добавляем таблицу соответствия
                st.subheader("Таблица кодирования")
                encoding_df = pd.DataFrame([
                    {"Число": x, "Инфо биты": _bin4(x), "Кодовое слово": coder.encode_symbol(x)}
                    for x in seq
                ])
                st.dataframe(encoding_df, use_container_width=True)

                # Отображаем метрики Хэмминга
                st.subheader("Основные характеристики кода Хэмминга (7,4)")
                m1, m2, m3 = st.columns(3)
                with m1:
                    st.metric("Кодовое расстояние", hamming_metrics['code_distance'])
                    st.metric("Длина кода", hamming_metrics['codeword_length'])
                    st.metric("Инфо биты", hamming_metrics['info_bits'])
                with m2:
                    st.metric("Исправляет ошибок", hamming_metrics['correctable_errors'])
                    st.metric("Обнаруживает ошибок", hamming_metrics['detectable_errors'])
                    st.metric("Избыточные биты", hamming_metrics['redundancy_bits'])
                with m3:
                    st.metric("Граница Хэмминга", "Выполнена" if hamming_metrics['hamming_bound'] else "Не выполнена")
                    st.metric("Граница Плоткина", hamming_metrics['plotkin_bound'])
                    st.metric("Граница Варшамова-Гильберта", hamming_metrics['varshamov_gilbert_bound'])

                # Пояснения к границам
                with st.expander("Пояснение к границам"):
                    st.write("""
                    **Граница Хэмминга:** Проверка совершенности кода. Код Хэмминга (7,4) является совершенным - достигает этой границы.
                    
                    **Граница Плоткина:** Определяет максимальное количество кодовых слов для заданных n и d.
                    
                    **Граница Варшамова-Гильберта:** Гарантирует существование кода с заданными параметрами.
                    """)

                st.subheader("Дополнительные характеристики")
                char1, char2 = st.columns(2)
                with char1:
                    st.metric("Совершенный код", "Да" if hamming_metrics['is_perfect_code'] else "Нет")
                    st.metric("Систематический код", "Да")
                with char2:
                    st.metric("Кодовых слов", hamming_metrics['number_of_codewords'])
                    st.metric("Скорость кода", f"{hamming_metrics['code_rate']}")

                # Таблица синдромов
                st.subheader("Таблица синдромов для декодирования")
                st.dataframe(pd.DataFrame(syndrome_table), use_container_width=True)

                # Показать все кодовые слова
                with st.expander("Показать все кодовые слова Хэмминга (7,4)"):
                    codewords_df = pd.DataFrame([
                        {"Десятичное": i, "Кодовое слово": word, "Вес Хэмминга": hamming_weight(word)}
                        for i, word in enumerate(hamming_metrics['all_codewords'])
                    ])
                    st.dataframe(codewords_df, use_container_width=True, height=400)

                st.download_button("Скачать закодированные данные (TXT)", data=encoded,
                                   file_name="hamming74_encoded.txt", mime="text/plain")

            except Exception as e:
                st.error(str(e))


def render_hamming_decoding():
    """
    Функция со страницей декодирования через алгоритм Хэмминга
    """
    st.header("Часть II: Декодирование по Хэммингу (7,4)")

    col1, col2 = st.columns(2)
    with col1:
        up = st.file_uploader("Файл с 0/1-последовательностью (кратно 7):", type=["txt"], key="hamming_decode_upload")
        default = "" if not up else up.read().decode("utf-8").strip()
        bits = st.text_area("Бинарная последовательность:", value=default, height=120)

    with col2:
        if st.button("Декодировать (Хэмминг 7,4)", type="primary"):
            try:
                coder = HammingCoder74()
                symbols, reports = coder.decode(bits)

                # Рассчитываем статистику исправлений
                total_blocks = len(reports)
                corrected_blocks = sum(1 for r in reports if r['was_corrected'])
                error_positions = [r['error_pos'] for r in reports if r['was_corrected']]

                st.success("Готово!")

                # Форматируем входные биты для читаемости
                formatted_input_bits = ' '.join([bits[i:i+7] for i in range(0, len(bits), 7)])
                st.text_area("Входная последовательность (разделена по 7 бит):",
                             formatted_input_bits, height=100)

                st.text_area("Декодированные числа:", " ".join(map(str, symbols)), height=100)

                # Статистика декодирования
                st.subheader("Статистика декодирования")
                stat1, stat2, stat3 = st.columns(3)
                with stat1:
                    st.metric("Всего блоков", total_blocks)
                with stat2:
                    st.metric("Исправлено ошибок", corrected_blocks)
                with stat3:
                    st.metric("Успешных блоков", total_blocks - corrected_blocks)

                st.subheader("Детальный отчёт по блокам")
                df = pd.DataFrame(reports)
                st.dataframe(df, use_container_width=True)

                # Показать распределение позиций ошибок
                if error_positions:
                    st.subheader("Распределение позиций ошибок")
                    error_df = pd.DataFrame([
                        {"Позиция ошибки": pos, "Тип бита": f"p{pos}" if pos > 4 else f"d{pos}", "Количество": error_positions.count(pos)}
                        for pos in set(error_positions)
                    ]).sort_values("Позиция ошибки")
                    st.dataframe(error_df, use_container_width=True)

                st.download_button("Скачать декодированные числа (TXT)",
                                   data=" ".join(map(str, symbols)), file_name="hamming74_decoded.txt", mime="text/plain")

                err_lines = [f"[{r['start']}:{r['end']}) syndrome={r['syndrome']} pos={r['error_pos']} corrected={r['was_corrected']}"
                             for r in reports if r['was_corrected']]
                err_txt = "\n".join(err_lines) or "Исправлений не было"
                st.download_button("Скачать отчёт об исправлениях (TXT)",
                                   data=err_txt, file_name="hamming74_corrections.txt", mime="text/plain")
            except Exception as e:
                st.error(str(e))


def hamming_weight(code_word):
    """
    Вспомогательная функция для вычисления веса Хэмминга
    :param code_word: кодовое слово в виде строки из 0 и 1
    """
    return sum(1 for bit in code_word if bit == '1')


if __name__ == "__main__":
    main()