import streamlit as st
from Generators_and_Testers import *


def parse_polynomial(poly_str: str) -> List[int]:
    """Парсит строку полинома в список коэффициентов"""
    poly_str = poly_str.strip().lower()

    # Удаляем 'x^' и пробелы, оставляем только коэффициенты
    terms = poly_str.replace('x^', '').replace('x', '1').split('+')

    # Находим максимальную степень
    degrees = []
    for term in terms:
        term = term.strip()
        if term.isdigit():
            degrees.append(int(term))
        elif 'x' in term:
            if term == 'x':
                degrees.append(1)
            else:
                coeff = term.replace('x', '')
                if coeff == '':
                    degrees.append(1)
                else:
                    degrees.append(int(coeff))

    if not degrees:
        return [1, 0, 0, 0, 0, 0, 0, 1]  # полином по умолчанию

    max_degree = max(degrees)

    # Создаем список коэффициентов
    coefficients = [0] * (max_degree + 1)
    coefficients[0] = 1  # старший коэффициент всегда 1

    for deg in degrees:
        if deg < len(coefficients):
            coefficients[deg] = 1

    return coefficients


def parse_input_file(file_content: str):
    """Парсит входной файл с параметрами"""
    lines = file_content.strip().split('\n')
    params = {}

    try:
        if len(lines) >= 5:
            # Парсим полиномы в коэффициенты
            poly_a_str = lines[0].strip()
            poly_s_str = lines[1].strip()

            params['poly_a'] = parse_polynomial(poly_a_str)
            params['poly_s'] = parse_polynomial(poly_s_str)
            params['poly_a_name'] = poly_a_str  # сохраняем оригинальное название для отображения
            params['poly_s_name'] = poly_s_str
            params['initial_a'] = lines[2].strip()
            params['initial_s'] = lines[3].strip()
            params['sequence_length'] = int(lines[4].strip())

        return params
    except Exception as e:
        st.error(f"Ошибка парсинга файла: {e}")
        return None


def main():
    st.set_page_config(
        page_title="Лабораторная работа №5 - Генерация ПСП",
        layout="wide"
    )

    st.title("Лабораторная работа №5")
    st.subheader("Генерирование равномерно распределенных псевдослучайных последовательностей")
    st.markdown("**Вариант 2:** Комбинирование с помощью псевдослучайного прореживания")

    # Инициализация session state
    if 'sequence' not in st.session_state:
        st.session_state.sequence = None
    if 'test_results' not in st.session_state:
        st.session_state.test_results = None
    if 'use_file' not in st.session_state:
        st.session_state.use_file = False
    if 'file_params' not in st.session_state:
        st.session_state.file_params = {}

    # Создаем вкладки
    tab1, tab2 = st.tabs([
        "Генерация ПСП",
        "Теоретическая справка и отчет"
    ])

    with tab1:
        st.header("Генерация псевдослучайных последовательностей")

        # Выбор способа ввода параметров
        input_method = st.radio(
            "Выберите способ ввода параметров:",
            ["Загрузить из файла", "Ввести вручную"],
            horizontal=True
        )

        use_file = (input_method == "Загрузить из файла")

        col1, col2 = st.columns(2)

        with col1:
            st.subheader("Параметры генераторов")

            if use_file:
                # Загрузка файла
                uploaded_file = st.file_uploader("Загрузите файл с параметрами (.txt)", type=["txt"])

                if uploaded_file is not None:
                    try:
                        file_content = uploaded_file.read().decode("utf-8")
                        params = parse_input_file(file_content)

                        if params:
                            st.session_state.file_params = params
                            st.success("Файл успешно загружен!")

                            # Показываем загруженные параметры
                            st.info(f"""
                            **Загруженные параметры:**
                            - Полином G₁: {params['poly_a']}
                            - Полином G₂: {params['poly_s']}
                            - Начальное состояние G₁: {params['initial_a']}
                            - Начальное состояние G₂: {params['initial_s']}
                            - Длина последовательности: {params['sequence_length']}
                            """)
                    except Exception as e:
                        st.error(f"Ошибка чтения файла: {e}")

                # Кнопка генерации для файлового режима
                if st.session_state.file_params:
                    if st.button("Сгенерировать последовательность из файла", type="primary"):
                        params = st.session_state.file_params
                        generate_from_params(params)

            else:  # Ручной ввод
                st.markdown("**Введите параметры вручную:**")

                # Словарь полиномов
                poly_options = {
                    "x⁷ + x³ + x² + x + 1": [1, 1, 1, 1, 0, 0, 0, 1],
                    "x⁷ + x + 1": [1, 1, 0, 0, 0, 0, 0, 1],
                    "x⁷ + x³ + 1": [1, 0, 0, 0, 1, 0, 0, 1]
                }

                poly_a_name = st.selectbox(
                    "Полином для генератора данных (G₁):",
                    list(poly_options.keys()),
                    index=0
                )
                poly_a = poly_options[poly_a_name]

                poly_s_name = st.selectbox(
                    "Полином для селектирующего генератора (G₂):",
                    list(poly_options.keys()),
                    index=1
                )
                poly_s = poly_options[poly_s_name]

                # Начальные состояния
                st.markdown("**Начальные состояния:**")
                initial_a = st.text_input("Начальное состояние G₁ (7 бит):", "1111111")
                initial_s = st.text_input("Начальное состояние G₂ (7 бит):", "1010101")

                # Длина последовательности
                sequence_length = st.slider(
                    "Длина выходной последовательности:",
                    min_value=100,
                    max_value=10000,
                    value=1000,
                    step=100
                )

                # Кнопка генерации для ручного режима
                if st.button("Сгенерировать последовательность", type="primary"):
                    params = {
                        'poly_a_name': poly_a_name,
                        'poly_a': poly_a,
                        'poly_s_name': poly_s_name,
                        'poly_s': poly_s,
                        'initial_a': initial_a,
                        'initial_s': initial_s,
                        'sequence_length': sequence_length
                    }
                    generate_from_params(params)

        with col2:
            st.subheader("Результаты генерации")
            display_results()

    with tab2:
        st.header("Отчет о работе")
        display_report()


def generate_from_params(params):
    """Генерация последовательности на основе параметров"""
    try:

        if (params['initial_a'].count('0') + params['initial_a'].count('1')) != len(params['initial_a']):
            st.error(f"Предупреждение! G1 Содержит нечто кроме 0 и 1")

        if (params['initial_s'].count('0') + params['initial_s'].count('1')) != len(params['initial_s']):
            st.error(f"Предупреждение! G2 Содержит нечто кроме 0 и 1")

        # Преобразуем начальные состояния
        initial_state_a = [int(bit) for bit in params['initial_a']]
        initial_state_s = [int(bit) for bit in params['initial_s']]

        # Проверяем длину начальных состояний
        if len(initial_state_a) != 7 or len(initial_state_s) != 7:
            st.error("Начальные состояния должны быть длиной 7 бит!")
            return

        # Создаем и запускаем генератор
        with st.spinner("Генерация последовательности..."):
            sg_generator = SGGenerator(
                params['poly_a'], initial_state_a,
                params['poly_s'], initial_state_s
            )
            sequence = sg_generator.generate_sequence(params['sequence_length'])

        # Тестируем последовательность
        with st.spinner("Тестирование последовательности..."):
            chi_square, critical_value, test_passed = PseudoRandomTester.chi_square_test(sequence)
            period_theory = PseudoRandomTester.find_period_theory(params['poly_a'], params['poly_s'])
            period_practice = PseudoRandomTester.find_period_practice(sequence)

        # Сохраняем результаты
        st.session_state.sequence = sequence
        st.session_state.test_results = {
            'chi_square': chi_square,
            'critical_value': critical_value,
            'test_passed': test_passed,
            'period_theory': period_theory,
            'period_practice': period_practice,
            'poly_a_name': params.get('poly_a_name', params.get('poly_a', 'Unknown')),
            'poly_s_name': params.get('poly_s_name', params.get('poly_s', 'Unknown')),
            'initial_a': params['initial_a'],
            'initial_s': params['initial_s'],
            'sequence_length': params['sequence_length']
        }

        # Сохраняем результаты в файлы
        save_results_to_files(sequence, st.session_state.test_results)

        st.success("Последовательность успешно сгенерирована и сохранена в файлы!")

    except Exception as e:
        st.error(f"Ошибка при генерации: {str(e)}")


def save_results_to_files(sequence, test_results):
    """Сохранение сгенерированной последовательности и результатов тестирования в файлы"""

    # Сохраняем саму последовательность
    sequence_str = ''.join(str(bit) for bit in sequence)
    with open("generated_sequence.txt", "w", encoding="utf-8") as f:
        f.write(sequence_str)

    # Сохраняем результаты тестирования
    with open("test_results.txt", "w", encoding="utf-8") as f:
        f.write("РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ ПОСЛЕДОВАТЕЛЬНОСТИ\n")
        f.write("=" * 50 + "\n\n")
        f.write(f"Длина последовательности: {len(sequence)}\n")
        f.write(f"Теоретический период T = (2ⁿ - 1) · 2ᵐ⁻¹: {test_results['period_theory']}\n")
        f.write(f"χ² статистика: {test_results['chi_square']:.4f}\n")
        f.write(f"Критическое значение: {test_results['critical_value']:.4f}\n")
        f.write(f"Тест равномерности: {'ПРОЙДЕН' if test_results['test_passed'] else 'НЕ ПРОЙДЕН'}\n")
        f.write(f"Полином G₁: {test_results['poly_a_name']}\n")
        f.write(f"Полином G₂: {test_results['poly_s_name']}\n")
        f.write(f"Начальное состояние G₁: {test_results['initial_a']}\n")
        f.write(f"Начальное состояние G₂: {test_results['initial_s']}\n")


def display_results():
    """Отображение результатов генерации"""
    if st.session_state.sequence is not None and st.session_state.test_results is not None:
        results = st.session_state.test_results
        sequence = st.session_state.sequence

        # Основные характеристики
        st.markdown("**Основные характеристики:**")
        col_a, col_b, col_c = st.columns(3)
        with col_a:
            st.metric("Длина", len(sequence))
        with col_b:
            st.metric("Теор. период", results['period_theory'])
        with col_c:
            practice_period = results['period_practice'] if results['period_practice'] != -1 else ">10000"
            st.metric("Прак. период", practice_period)

        # Детали теста
        st.markdown("**Детали χ²-теста:**")
        col_x, col_y = st.columns(2)
        with col_x:
            st.metric("χ² статистика", f"{results['chi_square']:.4f}")
        with col_y:
            st.metric("Критическое значение", f"{results['critical_value']:.4f}")

        # Кнопки для скачивания файлов
        st.markdown("**Скачать результаты:**")
        col1, col2 = st.columns(2)

        with col1:
            # Скачать последовательность
            sequence_str = ''.join(str(bit) for bit in sequence)
            st.download_button(
                label="Скачать последовательность (.txt)",
                data=sequence_str,
                file_name="generated_sequence.txt",
                mime="text/plain"
            )

        with col2:
            # Скачать результаты тестов
            test_report = f"""РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ ПОСЛЕДОВАТЕЛЬНОСТИ
Длина последовательности: {len(sequence)}
Период последовательности: {results['period_theory']}
χ² статистика: {results['chi_square']:.4f}
Критическое значение: {results['critical_value']:.4f}
Тест равномерности: {'ПРОЙДЕН' if results['test_passed'] else 'НЕ ПРОЙДЕН'}
Полином G₁: {results['poly_a_name']}
Полином G₂: {results['poly_s_name']}
Начальное состояние G₁: {results['initial_a']}
Начальное состояние G₂: {results['initial_s']}"""

            st.download_button(
                label="Скачать результаты тестов (.txt)",
                data=test_report,
                file_name="test_results.txt",
                mime="text/plain"
            )

    else:
        st.info("Сгенерируйте последовательность для просмотра результатов")


def display_report():
    """Отображение отчета"""
    st.markdown("""
    ### Теоретическая справка
    
    **SG-генератор (псевдослучайное прореживание):**
    - G₁ - генератор данных, создает последовательность {aₜ}
    - G₂ - селектирующий генератор, создает последовательность {sₜ}
    - Выходная последовательность {xₜ} включает биты aₜ, для которых sₜ = 1
    
    **Свойства:**
    - Период T = (2ⁿ - 1) · 2ᵐ⁻¹, где n и m - степени полиномов G₁ и G₂
    - Полиномы должны быть примитивными
    - Периоды должны быть взаимно простыми
    """)

    if st.session_state.sequence is not None and st.session_state.test_results is not None:
        results = st.session_state.test_results
        seq = st.session_state.sequence

        st.markdown(f"""
        ### Результаты тестирования
        
        **Параметры генерации:**
        - Длина последовательности: {len(seq)}
        - Полином G₁ (данные): {results['poly_a_name']}
        - Полином G₂ (селектор): {results['poly_s_name']}
        - Начальное состояние G₁: {results['initial_a']}
        - Начальное состояние G₂: {results['initial_s']}
        
        **Результаты тестов:**
        - Найденный период: {results['period_theory']}
        - χ² статистика: {results['chi_square']:.4f}
        - Критическое значение: {results['critical_value']:.4f}
        - Тест равномерности: {'ПРОЙДЕН' if results['test_passed'] else 'НЕ ПРОЙДЕН'}
        """)

        report = f"""ОТЧЕТ ПО ЛАБОРАТОРНОЙ РАБОТЕ №5
Генерирование равномерно распределенных псевдослучайных последовательностей
        
ПАРАМЕТРЫ ГЕНЕРАТОРА:
- Метод: комбинирование с помощью псевдослучайного прореживания
- Полином G₁: {results['poly_a_name']}
- Полином G₂: {results['poly_s_name']}
- Начальное состояние G₁: {results['initial_a']}
- Начальное состояние G₂: {results['initial_s']}
- Длина последовательности: {len(seq)}

РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ:
- Период последовательности: {results['period_theory']}
- χ² статистика: {results['chi_square']:.4f}
- Критическое значение: {results['critical_value']:.4f}
- Тест равномерности: {'ПРОЙДЕН' if results['test_passed'] else 'НЕ ПРОЙДЕН'}
"""
        st.download_button(
            label="Скачать отчет (.txt)",
            data=report,
            file_name="lab5_report.txt",
            mime="text/plain"
        )

    else:
        st.info("Сгенерируйте последовательность во вкладке 'Генерация ПСП' для просмотра отчета")


if __name__ == "__main__":
    main()
