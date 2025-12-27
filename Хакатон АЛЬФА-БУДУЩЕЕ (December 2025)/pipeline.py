import pandas as pd
from classes import *
import mpmath

# Загрузка данных
questions = pd.read_csv('data/questions_clean.csv', delimiter=',', encoding='utf-8')[:20]
websites = pd.read_csv('data/websites_updated.csv', delimiter=',', encoding='utf-8')[:20]
websites = websites.dropna(axis=0)

# Преобразование данных
transformer = DataTransformer(questions, websites)
questions_cleaned = transformer.transform_questions()
websites_cleaned = transformer.transform_websites()

# Создание предсказаний с transformers
pipeline = TransformersPredictionPipeline(pooling_method="cls")  # или "mean"

# Базовый подход
pairs_results = pipeline.fit_predict_pairs(questions_cleaned, websites_cleaned, k=5)

# Или расширенный подход
# pairs_results = pipeline.fit_predict_pairs_advanced(
#     questions_cleaned, websites_cleaned, k=5, weights=(0.5, 0.3, 0.2)
# )

# Создание финального датафрейма
result_df = pipeline.get_top_k_dataframe(pairs_results, k=5)

# Сохранение результатов
result_df.to_csv('submit.csv', index=False)

print("Результаты сохранены!")
print(result_df.head())