#include "grid.h"

//generatory liczb losowych
std::mt19937_64 generator(time(NULL));  //algorytm Mersenne Twister
//std::default_random_engine generator((unsigned int)time(0)); //generator LCG

int random(int r1, int r2)              //losuje liczbę z zakresu liczb r1 i r2
{
    std::uniform_int_distribution<> distribution(r1, r2);
    return distribution(generator);
}

Grid::Grid(int _size) : size(_size)
{
    grid.resize(size);
    for (int i = 0; i < size; i++)
        grid[i].resize(size);

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            grid[i][j] = new Hex(i, j);
            connect(grid[i][j], Hex::beginIsSet, this, Grid::selectEnd);
            connect(grid[i][j], Hex::endIsSet, this, Grid::setSelectable);
            connect(grid[i][j], Hex::beginIsSet, this, Grid::setBegin);
            connect(grid[i][j], Hex::endIsSet, this, Grid::setAim);
        }
    begin = grid[0][0];
    aim = grid[size - 1][size - 1];
}

Grid::~Grid()
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            delete grid[i][j];
}

Hex * Grid::XY(int x, int y)
{
    return grid[x][y];
}

void Grid::setSelectable()                  //umożliwia wybór sześcianu kliknięciem myszy
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            grid[i][j]->isSelectable();
}

void Grid::selectEnd()                      //informuje, że punkt koncowy zostal wybrany
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            grid[i][j]->endIsSelectable();
}

void Grid::setBegin(int _x, int _y)         //ustawia punkt początkowy
{
    beginIsSet = true;
    begin = grid[_x][_y];
}

void Grid::setAim(int _x, int _y)           //ustawia punkt koncowy
{
    aim = grid[_x][_y];
}

void Grid::reset_beg_end()                  //resetuje wartości punktów początkowego i końcowego
{
    beginIsSet = false;
}

void Grid::openRandom(int val)              //otwiera losową liczbę węzłów | val oznacza liczbę procentową
{
    int i, j;
    int total = size * size;
    val = (total * val) / 100;

    if (val <= openPrev)
    {
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                grid[i][j]->refresh();
        openPrev = 0;
    }
    else
        val -= openPrev;

    openPrev = val + openPrev;

    while (val > 0)
    {
        if (val < 0 || val > total)
        {
            emit grid[0][0]->statement(QString("Wartość nie może być mniejsza niż 0 i większa niż 100"));
            delay(2000);
            emit grid[0][0]->statement(QString("Wstaw przeszkody i wagi lub kliknij \"Szukaj\""));
            break;
        }
        i = random(0, size - 1);
        j = random(0, size - 1);
        if (grid[i][j]->getForce() != 0)
            continue;
        else
            grid[i][j]->setForce();
        --val;
    }
}

void Grid::percolation()                    //wyszukiwanie progu perkolacji, algorytm "perkolacja"
{
    QElapsedTimer timer;
    killLoopFlag = false;
    bool left = false; bool right = false; bool up = false; bool down = false;
    int total = size * size;
    double treshold;
    srand(unsigned(time(0)));
    int nX, nY, currentX, currentY, direction;
    int count = 0;
    int open = 0;

    if (grid[19][19]->mode == 2)
    {
        nX = begin->getX(); nY = begin->getY();
    }
    else
    {
        nX = 19; nY = 19;
    }
    emit grid[nX][nY]->statement(QString("Szukam progu perkolacji"));

    timer.start();

    while (!(up && left && right && down))
    {
        currentX = nX; currentY = nY;
        direction = rand() % 6;
        neighbour = grid[nX][nY]->findNeighbour(direction);
        nX = neighbour[0]; nY = neighbour[1];

        while (nX < 0 || nX > size - 1 || nY < 0 || nY > size - 1 || grid[nX][nY]->mode == 2)
        {
            direction = rand() % 6;
            neighbour = grid[currentX][currentY]->findNeighbour(direction);
            nX = neighbour[0]; nY = neighbour[1];
        }

        if (grid[nX][nY]->isVisited()) {}
        else
            open++;


        grid[nX][nY]->percolate_open();

        if (nX == 0)
            up = true;
        if (nY == 0)
            left = true;
        if (nX == size - 1)
            right = true;
        if (nY == size - 1)
            down = true;

        count++;

        emit sendSteps(count);
        emit sendTime((double)timer.elapsed() / 1000);

        QApplication::processEvents();
        if (killLoopFlag == true)
            break;
    }

    treshold = ((double)open/(double)total);

    emit grid[nX][nY]->statement(QString("Próg perkolacji ") + QString::number(treshold, 'g', 5));
    treshold = 0;
    open = 0;
}

void Grid::find_path_random()               //wyszukiwanie punktu końcowego | algorytm błądzenie losowe
{
    QElapsedTimer timer;
    killLoopFlag = false;
    srand(unsigned(time(0)));
    int nX, nY, currentX, currentY, direction;
    int count = 0;

    if (!beginIsSet)
    {
        grid[0][0]->setBegin();
        grid[size - 1][size - 1]->setEnd();
    }

    nX = begin->getX(); nY = begin->getY();
    emit grid[nX][nY]->statement(QString("Szukam drogi - generator liczb pseudolosowych!"));

    timer.start();

    while (aim->endNeighbour(nX, nY))
    {
        currentX = nX; currentY = nY;
        direction = rand() % 6;
        neighbour = grid[nX][nY]->findNeighbour(direction);
        nX = neighbour[0]; nY = neighbour[1];

        while (nX < 0 || nX > size - 1 || nY < 0 || nY > size - 1 || grid[nX][nY]->mode == 2)
        {
            direction = rand() % 6;
            neighbour = grid[currentX][currentY]->findNeighbour(direction);
            nX = neighbour[0]; nY = neighbour[1];
        }

        grid[nX][nY]->setToOpen();
        count++;
        emit sendSteps(count);
        emit sendTime((double)timer.elapsed() / 1000);

        QApplication::processEvents();
        if (killLoopFlag == true)
            break;
    }

    emit grid[nX][nY]->statement(QString("Znaleziono!"));
}

void Grid::find_path_force()                //wyszukiwanie punktu końcowego | algorytm błądzenie losowe z wagą
{
    QElapsedTimer timer;
    killLoopFlag = false;
    QVector<int> ftbl;                      //tablica przechowuje wartosci wag sasiada
    int nX, nY, currentX, currentY, direction;
    int probability;
    int count = 0;

    if (!beginIsSet)
    {
        grid[0][0]->setBegin();
        grid[size - 1][size - 1]->setEnd();
    }

    nX = begin->getX(); nY = begin->getY(); //poczatkowo wspolrzedne sasiada sa rowne poczatkowi
    emit grid[nX][nY]->statement(QString("Szukam drogi - generator liczb pseudolosowych z wagą!"));

    timer.start();

    while (aim->endNeighbour(nX, nY))       //tak dlugo jak punkt nie bedzie sasiadowal z aim
    {
        currentX = nX; currentY = nY;
        for (int i = 0; i < 6; i++)         //zbiera dane o wadze sasiadow do tablicy
        {
            neighbour = grid[currentX][currentY]->findNeighbour(i);
            nX = neighbour[0]; nY = neighbour[1];
            if (nX < 0 || nX > size - 1 || nY < 0 || nY > size - 1 || grid[nX][nY]->mode == 2)
            {
                nX = currentX; nY = currentY;
                ftbl.push_back(-1);
            }
            else
                ftbl.push_back(grid[nX][nY]->getForce());
        }

                                            //na tym etapie mamy tablicę z indeksami sasiadow, i ich wagami
        probability = random(0, 100);

        if (probability <= 5)                                          //5%
            direction = force_random(ftbl, 1);
        else if (probability > 5 && probability <= 20)                 //15%
            direction = force_random(ftbl, 2);
        else if (probability > 20 && probability <= 40)                //20%
            direction = force_random(ftbl, 3);
        else if (probability >= 40 && probability <= 100)              //60%
            direction = force_random(ftbl, 4);

        while (ftbl[direction] == -1)
             direction = random(0, 5);

        neighbour = grid[currentX][currentY]->findNeighbour(direction);
        nX = neighbour[0]; nY = neighbour[1];

        grid[nX][nY]->setNeighbour();       //przejdź do kolejnego sześcianu o współrzędnych nX, nY
        count++;
        emit sendSteps(count);
        emit sendTime((double)timer.elapsed() / 1000);

        ftbl.erase(ftbl.begin(), ftbl.end());

        QApplication::processEvents();
        if (killLoopFlag == true)
            break;
    }
    if (count > 99999)
        qDebug() << count;

    emit grid[nX][nY]->statement(QString("Znaleziono!"));
}

void Grid::find_path_force_unique()         //wyszukiwanie punktu końcowego | algorytm błądzenie losowe z wagą - wariant drugi
{
    QElapsedTimer timer;
    killLoopFlag = false;
    QVector<int> ftbl;                      //tablica przechowuje wartosci wag sasiada
    int nX, nY, currentX, currentY, direction;
    int probability;
    int count = 0;
    int clogged = 0;

    if (!beginIsSet)
    {
        grid[0][0]->setBegin();
        grid[size - 1][size - 1]->setEnd();
    }

    nX = begin->getX(); nY = begin->getY(); //poczatkowo wspolrzedne sasiada sa rowne poczatkowi
    emit grid[nX][nY]->statement(QString("Szukam drogi - generator liczb pseudolosowych z wagą - wariant drugi!"));

    timer.start();

    while (aim->endNeighbour(nX, nY))       //tak dlugo jak punkt nie bedzie sasiadowal z aim
    {
        currentX = nX; currentY = nY;
        for (int i = 0; i < 6; i++)         //zbiera dane o wadze sasiadow do tablicy
        {
            neighbour = grid[currentX][currentY]->findNeighbour(i);
            nX = neighbour[0]; nY = neighbour[1];

            if (nX < 0 || nX > size - 1 || nY < 0 || nY > size - 1 || grid[nX][nY]->mode == 2)
            {
                nX = currentX; nY = currentY;
                ftbl.push_back(-1);
            }
            else if (grid[nX][nY]->isVisited())
            {
                ftbl.push_back(5);      //jesli zostal odwiedzony dodaj do tablicy 5 i zwieksz clogged
                clogged++;
            }
            else
                ftbl.push_back(grid[nX][nY]->getForce());

        }

        if (clogged == 6)               //jeśli węzeł otoczony jest tylko sąsiadami o największych wagach
            std::replace(ftbl.begin(), ftbl.end(), 5, 4);
                                                    //na tym etapie mamy tablicę z indeksami sasiadow, i ich wagami
        probability = random(0, 100);

        if (probability <= 5)                                          //5%
            direction = force_random_unique(ftbl, 1);
        else if (probability > 5 && probability <= 15)                 //10%
            direction = force_random_unique(ftbl, 2);
        else if (probability > 15 && probability <= 40)                //25%
            direction = force_random_unique(ftbl, 3);
        else if (probability >= 40 && probability <= 100)              //60%
            direction = force_random_unique(ftbl, 4);

        while (ftbl[direction] == -1 || ftbl[direction] == 5)
            direction = random(0, 5);

        neighbour = grid[currentX][currentY]->findNeighbour(direction); //szukaj sąsiada w kierunku direction
        nX = neighbour[0]; nY = neighbour[1];

        grid[nX][nY]->setNeighbour();               //przejdź do kolejnego sześcianu o współrzędnych nX, nY
        count++;
        emit sendSteps(count);
        emit sendTime((double)timer.elapsed() / 1000);
        clogged = 0;

        ftbl.erase(ftbl.begin(), ftbl.end());

        QApplication::processEvents();
        if (killLoopFlag == true)
            break;
    }
    if (count > 99999)
        qDebug() << count;

    emit grid[nX][nY]->statement(QString("Znaleziono!"));
}

int Grid::force_random_unique(QVector<int> & tbl, int frc)  //wybiera indeks z tablicy, który przechowuje wagę "frc",
                                                            //wersja dla drugiego wartiantu błądzenia losowego z wagą
{
    int test = 0;
    auto result = std::find(tbl.begin(), tbl.end(), frc);
    if (result != std::end(tbl))
    {
        QVector<int> n;
        for (int i = 0; i < tbl.size(); i++)
            if (tbl[i] == frc)
                n.push_back(i);
        return n[random(0, n.size() - 1)];
    }
    else
    {
        for (int i = 0; i < tbl.size(); i++)
            if (tbl[i] == -1 || tbl[i] == 5)
                test++;
        if (test == 6)
        {
            std::replace(tbl.begin(), tbl.end(), 5, 4);
            return force_random(tbl, frc);
        }
        else
            return random(0, 5);
    }
}

int Grid::force_random(const QVector<int> & tbl, int frc)   //wybiera indeks z tablicy, który przechowuje wagę "frc",
                                                            //wersja dla błądzenia losowego z wagą
{
    auto result = std::find(tbl.begin(), tbl.end(), frc);
    if (result != std::end(tbl))
    {
        QVector<int> n;
        for (int i = 0; i < tbl.size(); i++)
            if (tbl[i] == frc)
                n.push_back(i);
        return n[random(0, n.size() - 1)];
    }
    else
        return random(0, 5);
}

void Grid::setForce(int f)                                  //ustaw wagę
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            grid[i][j]->setForceClicked(f);
}

