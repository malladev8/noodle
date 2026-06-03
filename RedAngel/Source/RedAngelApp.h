#pragma once
#include "NoodleApp.h"

class RedAngelApp : public NoodleApp
{
public:
	RedAngelApp(EngineContext& engineContext);
	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	virtual void Shutdown() override;

protected:
private:
};