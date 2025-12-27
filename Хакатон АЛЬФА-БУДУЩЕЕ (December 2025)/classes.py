import numpy as np
import pandas as pd
import re
import string
import nltk
from nltk.corpus import stopwords
import torch
import torch.nn.functional as F
from transformers import AutoTokenizer, AutoModel

nltk.download('stopwords')


def pool(hidden_state, mask, pooling_method="cls"):
    """Пулинг эмбеддингов"""
    if pooling_method == "mean":
        s = torch.sum(hidden_state * mask.unsqueeze(-1).float(), dim=1)
        d = mask.sum(axis=1, keepdim=True).float()
        return s / d
    elif pooling_method == "cls":
        return hidden_state[:, 0]
    elif pooling_method == "max":
        return torch.max(hidden_state * mask.unsqueeze(-1).float(), dim=1)[0]


class DataPreprocessor:
    """
    Класс для вспомогательных функций преобразования данных
    """
    def __init__(self):
        self.stop_words = set(stopwords.words('russian')) | {'здравствуйте'}
        self.translit_map = {
            'alfabank': 'альфабанк',
            'actions': 'акции',
            'rules': 'правила',
            'alfafuture': 'альфа будущее',
        }

    def clean_text(self, text: str) -> str:
        """Очистка текста"""
        text = str(text).lower()
        text = re.sub(f'[{re.escape(string.punctuation)}]', '', text)
        text = re.sub(r'[—«»]', '', text)
        text = re.sub(r'\n', ' ', text)
        text = text.replace('ё', 'е')
        text = ' '.join([word for word in text.split() if word not in self.stop_words and len(word) > 2])
        return text

    def translate_translit(self, text: str) -> str:
        """Преобразование транслита в кириллицу где возможно"""
        for eng, rus in self.translit_map.items():
            text = text.replace(eng, rus)
        return text

    def websites_url_splitting(self, url: str) -> str:
        """Разбиение URL на части с улучшенной обработкой"""
        if pd.isna(url):
            return ""

        url_parts = url.split('/')[3:]
        filtered_parts = [part for part in url_parts if 3 < len(part) < 50 and not part.startswith('2')]

        processed_parts = []
        for part in filtered_parts:
            part = part.replace('-', ' ')
            part = self.translate_translit(part)
            processed_parts.append(part)

        return ' '.join(processed_parts)


class DataTransformer(DataPreprocessor):
    """
    Класс для преобразования данных
    """
    def __init__(self, questions_df: pd.DataFrame, websites_df: pd.DataFrame):
        super().__init__()
        self.questions_df = questions_df.copy()
        self.websites_df = websites_df.copy()

    def transform_questions(self) -> pd.DataFrame:
        """Трансформация вопросов"""
        self.questions_df['query_cleaned'] = self.questions_df['query'].apply(self.clean_text)
        return self.questions_df

    def transform_websites(self) -> pd.DataFrame:
        """Трансформация веб-сайтов"""
        self.websites_df['title_cleaned'] = self.websites_df['title'].apply(self.clean_text)
        self.websites_df['text_cleaned'] = self.websites_df['text'].apply(self.clean_text)
        self.websites_df['url_cleaned'] = self.websites_df['url'].apply(self.websites_url_splitting)

        self.websites_df['website_all_data'] = (
                self.websites_df['title_cleaned'] + ' ' +
                self.websites_df['url_cleaned'] + ' ' +
                self.websites_df['text_cleaned']
        )

        return self.websites_df


class TransformersPredictionPipeline:
    """Класс с моделью на основе transformers"""
    def __init__(self, model_name="ai-forever/ru-en-RoSBERTa", pooling_method="cls"):
        print(f"Загрузка модели: {model_name}")
        self.tokenizer = AutoTokenizer.from_pretrained(model_name)
        self.model = AutoModel.from_pretrained(model_name)
        self.pooling_method = pooling_method
        self.model.eval()  # Переводим модель в режим оценки
        print("Модель загружена!")

    def encode_texts(self, texts: list) -> torch.Tensor:
        """Кодирование списка текстов в эмбеддинги"""
        print(f"Кодируем {len(texts)} текстов...")

        # Токенизация
        tokenized_inputs = self.tokenizer(
            texts,
            max_length=512,
            padding=True,
            truncation=True,
            return_tensors="pt"
        )

        # Получение эмбеддингов
        with torch.no_grad():
            outputs = self.model(**tokenized_inputs)

        # Применяем пулинг
        embeddings = pool(
            outputs.last_hidden_state,
            tokenized_inputs["attention_mask"],
            pooling_method=self.pooling_method
        )

        # Нормализуем эмбеддинги
        embeddings = F.normalize(embeddings, p=2, dim=1)

        return embeddings

    def fit_predict_pairs(self, questions_df: pd.DataFrame, websites_df: pd.DataFrame,
                          query_column='query_cleaned',
                          website_column='website_all_data',
                          k=5) -> list:

        print("Кодирование текстов и вычисление схожести...")

        # Получаем тексты для кодирования
        queries = questions_df[query_column].tolist()
        websites_texts = websites_df[website_column].tolist()

        # Сохраняем ID и URL для результатов
        self.question_ids = questions_df['q_id'].tolist()
        self.website_ids = websites_df['web_id'].tolist()

        print(f"Кодируем {len(queries)} вопросов...")
        questions_emb = self.encode_texts(queries)

        print(f"Кодируем {len(websites_texts)} сайтов...")
        websites_emb = self.encode_texts(websites_texts)

        print("Вычисляем схожесть...")

        # Вычисляем косинусную схожесть
        scores = questions_emb @ websites_emb.T  # Матрица схожести

        # Обрабатываем результаты
        results = []
        for i, query in enumerate(queries):
            query_scores = scores[i].cpu().tolist()

            # Комбинируем веб-сайты, URLs и scores
            doc_score_pairs = list(zip(self.website_ids, query_scores))

            # Сортируем по убыванию score
            doc_score_pairs = sorted(doc_score_pairs, key=lambda x: x[1], reverse=True)

            # Сохраняем топ-N результатов
            top_results = []
            for web_id, score in doc_score_pairs[:k]:
                top_results.append({
                    'web_id': web_id,
                    'score': score,
                })

            results.append({
                'q_id': self.question_ids[i],
                'web_list': top_results
            })

        print("Готово!")
        return results

    def fit_predict_pairs_advanced(self, questions_df: pd.DataFrame, websites_df: pd.DataFrame,
                                   k=5, weights=(0.5, 0.15, 0.35)) -> list:
        """
        Расширенный метод с раздельным кодированием разных типов текста
        """
        print("Расширенное кодирование текстов...")

        queries = questions_df['query_cleaned'].tolist()

        # Разделяем данные веб-сайтов
        titles = websites_df['title_cleaned'].tolist()
        url_texts = websites_df['url_cleaned'].tolist()
        main_texts = websites_df['text_cleaned'].tolist()

        self.question_ids = questions_df['q_id'].tolist()
        self.website_ids = websites_df['web_id'].tolist()
        self.website_urls = websites_df['url'].tolist()

        print("Кодируем разные типы данных...")

        # Кодируем раздельно
        queries_emb = self.encode_texts(queries)
        titles_emb = self.encode_texts(titles)
        urls_emb = self.encode_texts(url_texts)
        texts_emb = self.encode_texts(main_texts)

        print("Вычисляем комбинированную схожесть...")

        title_weight, url_weight, text_weight = weights

        results = []
        for i, query in enumerate(queries):
            # Вычисляем схожести для разных типов данных
            title_scores = (queries_emb[i] @ titles_emb.T).cpu().tolist()
            url_scores = (queries_emb[i] @ urls_emb.T).cpu().tolist()
            text_scores = (queries_emb[i] @ texts_emb.T).cpu().tolist()

            # Комбинируем scores с весами
            combined_scores = []
            for j in range(len(title_scores)):
                combined_score = (title_scores[j] * title_weight +
                                  url_scores[j] * url_weight +
                                  text_scores[j] * text_weight)
                combined_scores.append(combined_score)

            # Формируем результаты
            doc_score_pairs = list(zip(self.website_ids, combined_scores))
            doc_score_pairs = sorted(doc_score_pairs, key=lambda x: x[1], reverse=True)

            top_results = []
            for web_id, score in doc_score_pairs[:k]:
                top_results.append({
                    'web_id': web_id,
                    'score': score,
                })

            results.append({
                'q_id': self.question_ids[i],
                'web_list': top_results
            })

        return results

    def get_top_k_dataframe(self, pairs_results: list = None, k: int = 5) -> pd.DataFrame:
        """
        Создает датафрейм с id вопроса и топ-k наиболее подходящих ссылок
        """
        results = []
        for pair_result in pairs_results:
            q_id = pair_result['q_id']
            top_websites = pair_result['web_list'][:k]

            urls = [website['web_id'] for website in top_websites]

            results.append({
                'q_id': q_id,
                'web_list': urls
            })

        return pd.DataFrame(results)
