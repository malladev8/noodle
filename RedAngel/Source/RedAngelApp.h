#pragma once
#include "NoodleApp.h"

class RedAngelApp : public NoodleApp
{
public:
	RedAngelApp(EngineContext& engineContext);
	virtual void Init() override;
	virtual void Run(float deltaSeconds) override;
	virtual void Shutdown() override;

protected:
private:
};