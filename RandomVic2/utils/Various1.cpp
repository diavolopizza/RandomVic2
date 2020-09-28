#include "Various1.h"


Various1::Various1()
{
	//orderedprov.resize(numprov);
	//provs.resize(boost::extents[256][256][256]);
}

Various1::~Various1()
{
}

//void Various1::provPixels(int filesize)
//{
//	int r, g, b;
//
//	BYTE* data = new unsigned char[filesize];
//
//	data = BMPHandler::getInstance().findBitmapByKey("provinces")->Buffer;
//
//	for (int j = 0; j < filesize * 3 - 30; j += 30)
//	{
//		try {
//			r = data[j + 2];
//			g = data[j + 1];
//			b = data[j];
//			provs[r][g][b]->pixels.insert(j / 3);
//		}
//		catch (runtime_error e)
//		{
//			cout << r << " " << g << " " << b << endl;
//		}
//	}
//}
//
//boost::multi_array<Prov*, 3> Various1::provhashes()
//{
//	string temp1;
//	ifstream defi;
//	int pos = 0;
//	getline(defi, temp1);
//	int r, g, b, provnr;
//	defi.open( HoiPath + "map/definition.csv");
//	for (unsigned int i = 0; i < numprov; i++)
//	{
//		getline(defi, temp1);
//		pos = temp1.find_first_of(';', pos + 1);
//		provnr = stoi(temp1.substr(0, temp1.find_first_of(';')));
//
//		r = stoi(temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1)));//RED
//
//		pos = temp1.find_first_of(';', pos + 1);
//		g = stoi(temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1)));//GREEN
//
//		pos = temp1.find_first_of(';', pos + 1);
//		b = stoi(temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1)));//BLUE
//
//		Prov*prov = new Prov(provnr, r, g, b);
//
//		pos = temp1.find_first_of(';', pos + 1);
//		string temp2 = temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1) - pos - 1);//land, sea or lake
//		if (temp2 == "sea" || temp2 == "lake")
//			prov->sea = true;
//		else {
//			prov->sea = false;
//		}
//
//		pos = temp1.find_first_of(';', pos + 1);
//		if (temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1) - pos - 1) == "true")//coastal
//			prov->coastal = true;
//		else
//			prov->coastal = false;
//
//
//
//		pos = temp1.find_first_of(';', pos + 1);
//		prov->terrain_type = temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1) - pos - 1); //terraintype
//
//		pos = temp1.find_first_of(';', pos + 1);
//		prov->continent = stoi(temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1) - pos - 1));//continent
//
//		pos = 0;
//		orderedprov[prov->provnr] = prov;
//		provs[prov->r][prov->g][prov->b] = prov;
//	}
//	return provs;
//}
//
//void Various1::writeadj()
//{
//	string all;
//	for (auto prov : orderedprov)
//	{
//		all.append('\n' + to_string(prov->provnr) + ';');
//		all.append(to_string(prov->sea) + ';');
//		//all.append(to_string(z->island) + ';');
//
//		all.append(to_string(prov->r) + ';');
//
//		all.append(to_string(prov->g) + ';');
//
//		all.append(to_string(prov->b) + ';');
//		for (auto q : prov->neighbours)
//			all.append(to_string(q->provnr) + ';');
//	}
//
//	ofstream adj;
//	adj.open("sourcefiles/mapsource2/adj.txt");
//	adj << all;
//	adj.close();
//}
//
//void Various1::readadj(string source)
//{
//	fstream adj;
//	string line;
//	adj.open(source);
//	getline(adj, line, '\n');
//	while (!adj.eof())
//	{
//		int pos = 0;
//		getline(adj, line, '\n');
//
//
//		int provnr = stoi(line.substr(0, line.find_first_of(';', pos) - pos));
//
//		pos = line.find_first_of(';', pos) + 1;
//		orderedprov[provnr]->sea = stoi(line.substr(pos, line.find_first_of(';', pos) - pos)) != 0;
//		pos = line.find_first_of(';', pos) + 1;
//		//P->island = stoi(line.substr(pos, line.find_first_of(';', pos) - pos));
//
//		//pos = line.find_first_of(';', pos) + 1;
//		orderedprov[provnr]->r = stoi(line.substr(pos, line.find_first_of(';', pos) - pos));
//		pos = line.find_first_of(';', pos) + 1;
//		orderedprov[provnr]->g = stoi(line.substr(pos, line.find_first_of(';', pos) - pos));
//		pos = line.find_first_of(';', pos) + 1;
//		orderedprov[provnr]->b = stoi(line.substr(pos, line.find_first_of(';', pos) - pos));
//		while (line.find_first_of(';', pos) != line.size() - 1)
//		{
//			pos = line.find_first_of(';', pos) + 1;
//			orderedprov[provnr]->neighbours.insert(orderedprov[(stoi(line.substr(pos, line.find_first_of(';', pos) - pos)))]);
//		}
//		if (orderedprov[provnr]->sea == true)
//			orderedprov[provnr]->placed = true;
//		else orderedprov[provnr]->placed = false;
//
//
//		provs[orderedprov[provnr]->r][orderedprov[provnr]->g][orderedprov[provnr]->b] = orderedprov[provnr];
//	}
//}
