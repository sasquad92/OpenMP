#include <time.h>
#include <iostream>
#include <omp.h>
#include <math.h>
#include <fstream>
#include <chrono>


using namespace std;

double begin_t, end_t; //do zliczania czasu

float **tab;

long double det = 1.0;
long double detMP = 1.0;
long i, j, k, N;
bool once = true;
int ilosc_watkow = 0;
int wielkosc=0;

fstream macierz;

// Losowanie macierzy o zadanym wymierze + zapis do pliku (jezeli nie zakomentowane)
int LosowanieMacierzy()
{
	cout << "Podaj rozmiar macierzy [NxN]: ";
	cin >> N;

	//wypelnienie macierzy losowymi liczbami
	time_t t;
	srand((unsigned)time(&t));
	tab = new float*[N];

	////macierz.open("macierz.txt", ios::out);

	//cout << "Zawartosc macierzy: " << endl << endl;

	for (i = 0; i < N; i++)
	{
		tab[i] = new float[N];
	}

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			tab[i][j] = rand() % 10;

			////macierz << tab[i][j] << " ";
			cout << tab[i][j] << " ";

			if (j == N - 1)
			{
				////macierz << endl;
				cout << endl;
			}

		}
	}

	////macierz.close();
	////macierz.clear();
	return N;
}

//odczyt z pliku macierzy o konkretnym wymiarze
int OdczytMacierzy()
{
	if (macierz.good() == true)
	{
		//N = 100;
		tab = new float*[N];
		
		char wartosc;

		macierz.open("macierz.txt", ios::in);

		for (i = 0; i < N; i++)
		{
			tab[i] = new float[N];
		}

		for (i = 0; i < N; i++)
		{
			for (j = 0; j < N; j++)
			{
				macierz >> tab[i][j];

				cout << tab[i][j] << " ";
				if (j == N - 1)
					cout << endl;
			}
		}
		
		macierz.close();
		return N;
	}
	else
		cout << "Plik z macierza nie istnieje! Skontaktuj sie z administratorem systemu." << endl;
	return N;
}

//petla z obliczeniami
void Obliczenia()
{
	det = 1.0; // bez tego wartoœæ wyznacznika ca³y czas inkrementuje siê w pêtli - a obliczenia robione s¹ 999 razy.

	// tworzenie macierzy trojkatnej
	for (k = 0; k < N - 1; k++)
	{
		//#pragma omp parallel for shared(k, N) private(i, j)
		for (i = k + 1; i < N; i++)
		{ 
			for (j = N - 1; j >= k; j--)
			{
				tab[i][j] = tab[i][j] - tab[i][k] * tab[k][j] / tab[k][k]; // round()
			}
		}
	}	

	//if ( N < 5)
	//{
	//	cout << endl << "Powstala macierz: " << endl;

	//	//wyœwietlenie macierzy trójk¹tnej
	//	for (i = 0; i < N; i++)
	//	{
	//		for (j = 0; j < N; j++)
	//		{
	//			cout << tab[i][j] << " ";
	//			//printf("%.2f ", tab[i][j]);

	//			if (j == N - 1)
	//			{
	//				cout << endl;
	//			}

	//		}
	//	}
	//}
	
		for (i = 0; i < N; i++)
		{
			det = det* tab[i][i];
		}
}

void ObliczeniaOpenMP()
{

	detMP = 1.0; // bez tego wartoœæ wyznacznika ca³y czas inkrementuje siê w pêtli - a obliczenia robione s¹ 999 razy.

	// tworzenie macierzy trojkatnej
	for (k = 0; k < N - 1; k++)
	{
		#pragma omp parallel for default(shared) private(i, j) // k musi byæ wspólne, inaczej program siê nie wokona
		for (i = k + 1; i < N; i++)
		{
			for (j = N - 1; j >= k; j--)
			{
				tab[i][j] = tab[i][j] - tab[i][k] * tab[k][j] / tab[k][k];
			}
			if (once)
			{
				ilosc_watkow = omp_get_num_threads();
				once = false;
			}
		}
	}

	#pragma omp parallel for default(none) shared(N, tab) private(i) \
	reduction(* : detMP)
	for (i = 0; i < N; i++)
	{
		detMP = detMP* tab[i][i];
	}
}

//pomiar czasu z wywolaniem funkcji Obliczenia()
void PomiarCzasu( int x)
{
	double czasObliczenBezPrzyspieszenia = 0;
	double czasObliczenZPrzyspieszeniem = 0;
	
	//czêœæ programu do testów porównawczych
	/*begin_t = omp_get_wtime();

	for (int i = 0; i < 1000; i++)
	{
		Obliczenia();
	}

	end_t = omp_get_wtime();

	czasObliczenBezPrzyspieszenia = end_t - begin_t;

	begin_t = omp_get_wtime();

	for (int i = 0; i < 1000; i++)
	{
		ObliczeniaOpenMP();
	}

	end_t = omp_get_wtime();

	czasObliczenZPrzyspieszeniem = end_t - begin_t;

	cout << endl << "Wyznacznik wynosi: " << det << endl;
	cout << endl << "Czas obliczen bez przyspieszenia: " << czasObliczenBezPrzyspieszenia << endl;
	cout << endl << "Czas obliczen z przyspieszeniem: " << czasObliczenZPrzyspieszeniem << endl;
	*/
	if (x <= 50)
	{
		begin_t = omp_get_wtime();

		for (int i = 0; i < 1000; i++)
		{
			Obliczenia();
		}

		end_t = omp_get_wtime();

		czasObliczenBezPrzyspieszenia = end_t - begin_t;
		cout << endl << "Wielkosc macierzy ponizej 50x50, wykonywanie obliczen bez przyspieszenia";
		cout << endl << "Wyznacznik wynosi: " << det << endl;
		cout << endl << "Czas obliczen bez przyspieszenia: " << czasObliczenBezPrzyspieszenia << endl;
		cout << endl << "Ilosc wykorzystanych do obliczen watkow: 1" << endl;
	}else
		{
		begin_t = omp_get_wtime();

		for (int i = 0; i < 1000; i++)
		{
			ObliczeniaOpenMP();
		}

		end_t = omp_get_wtime();

		czasObliczenZPrzyspieszeniem = end_t - begin_t;
		cout << endl << "Wielkosc macierzy powyzej 50x50, wykonywanie obliczen z przyspieszeniem";
		cout << endl << "Wyznacznik wynosi: " << det << endl;
		cout << endl << "Czas obliczen z przyspieszeniem: " << czasObliczenZPrzyspieszeniem << endl;
		cout << endl << "Ilosc wykorzystanych do obliczen watkow: " << ilosc_watkow << endl;
	
	}
	
	cout << "KONIEC";
}

int main()
{
	char x;

	cout << "Macierz ma byc losowana (dowolny rozmiar) czy z pliku (100x100)?" << endl
		<< "1) Losowana" << endl
		<< "2) Z pliku" << endl;
	cin >> x;
	cout << endl;

	if (x == '2')
	{
		N = 100;
	}

	switch (x)
	{
	case '1':

		wielkosc=LosowanieMacierzy();
		PomiarCzasu(wielkosc);
		break;

	case '2':

		wielkosc=OdczytMacierzy();
		PomiarCzasu(wielkosc);
		break;

	default:

		cout << "Zly wybor, zamykam..." << endl;
		break;
	}

	//czyszczenie

	for (int i = 0; i < N; i++)
	{
		delete [] tab[i];
	}

	delete tab;

	cin.sync();
	cin.get();

	return 0;
}