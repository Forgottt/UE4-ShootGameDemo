// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Common/GameHMsg.h"
#include "ProNetWork.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DelegateCombinations.h"
#include "ProNetLogic.generated.h"

DEFINE_LOG_CATEGORY_STATIC(GameLog,Error,All);
/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLoginDelegate,bool,LoginOK);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRegistDelegate, bool, RegistOK);

UCLASS(Blueprintable, BlueprintType)
class MYPROJECT_API UProNetLogic : public UObject
{
	GENERATED_BODY()
private:
	~UProNetLogic();
	static UProNetLogic * m_pNetLogic;
	bool m_isConneted;

	FOnLoginDelegate m_OnLoginDelegate;
	FOnRegistDelegate m_OnRegistDelegate;
public:
	UFUNCTION(BlueprintCallable, Category = "NetLogic")
	static UProNetLogic * GetInstance();

	UFUNCTION(BlueprintCallable, Category = "NetLogic")
	void Connect(FString IP, int Port);

	UFUNCTION(BlueprintCallable, Category = "NetLogic")
	void GameLogicRegister(FString account, FString password);

	UFUNCTION(BlueprintCallable, Category = "NetLogic")
	void GameLogicLogin(FString account, FString password);	

	UFUNCTION(BlueprintCallable, Category = "NetLogic")
	void OnLogin(FOnLoginDelegate onlogin);

	UFUNCTION(BlueprintCallable, Category = "NetLogic")
	void OnRegister(FOnRegistDelegate onregist);

	UFUNCTION(BlueprintCallable, Category = "NetLogic")
	void DealMsg();
};
