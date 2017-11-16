#include "InstanceManager.h"
#include "Model.h"

#include <map>
#include <memory>
#include <string>

using namespace std;

typedef unique_ptr<Model> ModelUP;

struct InstanceManager::impl {
	map<string, ModelUP> models;
	
};

InstanceManager::InstanceManager()
{
	pImpl = new impl();

}


InstanceManager::~InstanceManager()
{
	delete pImpl;
}

Model * InstanceManager::GetModel(std::string path)
{
	// iterator
	auto it = pImpl->models.find(path);
	
	if (it != pImpl->models.end()) {
		return pImpl->models[path].get();
	}

	Model* model = new Model(path);
	
	pImpl->models.insert(pair<string, ModelUP>(path, ModelUP(model)));
	return model;
}


