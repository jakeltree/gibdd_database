# База данных штрафов ГИБДД
Реляционная база данных для хранения штрафов ГИБДД и клиент/сервер для доступа к базе.
## Описание базы данных
### Поля
- Владелец ФИО
- Марка машины,
- Гос. список штрафов
- Штрафы с датой выдачи, суммой штрафа и датой погашения
### Запросы к базе:
- Поставить на учет или снять с учета автомобиль
- Выдать штраф, погасить штраф.
- Статистика по наличию, количеству и сумме штрафов по автовладельцам, регионам, отрезкам времени, маркам машин и т.д.

## Этапы выполнения работы
- Папка generator - генератор CSV файла с данными для ГИБДД (1 этап).
- Папка database - рабочая база данных для ГИБДД (2-3 этап).
- Папки client, server - рабочий tcp клиент и сервер для ГИБДД (4 этап).
- scen.sh, client{1..4}, server - автоматизированная работа нескольких клиентов (этап 5).
- server http - сервер и клиент для http сервера с браузерным фронтендом (6 этап).
  
