#pragma once


template <class T> class MenuOption {
private:
	T value;
public:
	virtual	void Inc();
	virtual void Dec();
	virtual const char *Desc() {
		return "Menu Item";
	}
	virtual const char *DisplayValue() {
		return ""
	}

};
