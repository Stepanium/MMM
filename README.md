# MMM
Memory Maze Man!

01.09.2023
Света:
1. Выглядит, что проходишь сквозь стены
2. Цвета плохо сочетаются, под мелодию подходят вот такие https://youtu.be/QdabIfmcqSQ?feature=shared. есть грязные, обелённые и чистые как минимум
3. слишком большой лабиринт для начинающего
4. Нет смысла приближать камеру до режима Прохождения, тк всё равно из-за угла обзора проходить всю карту до конца не будут
5. Выход в Режиме изучения телепортирует к старту
6. выделить персонажа

Суть в чём.
Главная колонна - вызов
Он выражается в трёх частях (двух):
1. Игровой цикл
    1.1. Изучение карты
    1.2. Повторное прохождение карты с более ограниченной информацией
2. Развитие (смог запомнить и пройти такой тип лабиринта? (таких размера, сложности, тд...) А сможешь пройти более сложный вариант?)

Это не о том, чтобы сравнивать себя с другими, а о чёткой и ёмкой центральной механике, создающей условия для образования состояния течения. Как только ты одного размера допустим прошёл карту, следующая уже будет больше, тебе постоянно надо реорганизвать уже имеющиеся навыки на ходу в процессе сталкивания с экспоненциально усложняющимися задачами.

То есть самые главные пункты:
1. Цикл изучение - прохождение по памяти
2. Управление игроком
3. Физика столкновения со стенами
4. Ухудшение видимости после изучения, изначальная ограниченность обзора, чтобы сразу приходилось на память полагаться

Всё. Дальше уже на этом всё строится
У меня уже есть
1. физика
2. генерация лабиринта
3. передвижение игрока, управление
4. ограничение прорисовки ячеек в опр радиусе от персонажа
5. оторажение геймплея

осталось:
переключение между режимами: режим изучение + клавиша = режим прохождения по памяти. Вернуться в режим изучения тв можешь только закончив уровень.
приближение камеры

после этого уже пойдут
введение игрока в курс дела, заманивание его в игровой цикл
маленькие графические улучшения (плавное затемнение, мягкость перемещения камеры, более выразительная графика)
управление?


2д вид с верху
ты двигаешь персонажа в лабиринте
Когда ты готов, ты встаёшь у входа лабиринта и нажимаешь клавишу
Всё вокруг тебя темнеет, ты видишь только капельку стен в небольшом радиусе от тебя
тебе нужно снова найти выход

Со временем можно увеличивать сложность:
Добавить монстров
Ускорить/усилить монстров
Увеличить количество монстров
Увеличить количество типов монстров
поменять лабиринт
расширить лабиринт
уменьшить облать видимости
добавить мешающие графические эффекты: вращение экрана, эффект волны, яркие мелькающие цвета и т.д.
добавить мешающие аудиальные эффекты: случайные звуки самого сильного монстра, электронный шум/музыка, подсказки (то помогающие, то мешающие) и т.п.
добавить стрельбу и соответственно непроходимые без стрельбы препятствия
оружие ближнего боя
фальшивые стены
отвлекание монстров
эхо и стерео звук

самая суть: лабиринт, управление персонажем с видом сверху, изменение освещённости