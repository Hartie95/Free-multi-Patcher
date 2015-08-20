#pragma once

#include <3ds.h>

class MenuM{
};

class MenuManagerM{
	public: 
		virtual void back(){
		}
		virtual void navigateTo(MenuM* targetPage){
		}
};
