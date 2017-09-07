#include "Prov.h"


Prov::Prov()
{
}

Prov::Prov(int provnr,int r, int g,int b)
{
	this->provnr = provnr;
	this->r = r;
	this->g = g;
	this->b = b;
	this->tag = "DUM";
	this->stateid = 0;
}

Prov::Prov(int provnr, int r, int g, int b,bool s)
{
	this->provnr = provnr;
	this->r = r;
	this->g = g;
	this->b = b;
	this->tag = "DUM";
	this->sea = s;
	this->stateid = 0;
}

Prov::~Prov()
{
}


bool Prov::operator==(const Prov& right) const
{
	
	if (this->r == right.r)
		if (this->g == right.g)
			if (this->b == right.b)
				return true;


	return false;

}



void Prov::setneighbour2(Prov*P)
{
	neighbours.insert(P);
	P->neighbours.insert(this);

	//this->totalborder += 1;
	//if (this->neighbour.size() == 0)
	//{
	//	this->neighbour.push_back(neighbour);
	//	//this->bordertoeach.push_back(0);
	//}
	//else
	//for (int i = 0; i < this->neighbour.size(); i++)
	//{
	//	//
	//	//if (this->neighbour[i] == neighbour)
	//	//{
	//	//	this->bordertoeach[i] += 1;
	//	//	break;
	//	//}
	//	if (i >= this->neighbour.size() - 1)
	//	{
	//		this->neighbour.push_back(neighbour);
	//		//this->bordertoeach.push_back(0);
	//		break;
	//	}
	//}
	
}

void Prov::checkDeveloped(vector <int> developed_continent) {
	for (auto dev_con : developed_continent)
	{
		if (this->continent == dev_con)
		{
			this->developed = true;
			break;
		}
	}
}