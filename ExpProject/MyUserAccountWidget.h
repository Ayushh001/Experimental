 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "MyUserAccountWidget.generated.h"

/**
 * Account client class, handle:
 * Login
 * Register
 * Get account characters
 * Create new character
 */

USTRUCT(BlueprintType)
struct FLoginResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	TArray<FString>Roles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	FString AccessToken;


};

USTRUCT(BlueprintType)
struct FRegisterResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	FString Email;

};

USTRUCT(BlueprintType)
struct FAccountCharacterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	int32 Xp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	FString AccountName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	TMap<FString, FString> AppearanceInfo;
};

USTRUCT(BlueprintType)
struct FAccountCharacterResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AccountClient")
	TArray<FAccountCharacterBase> CharacterList;

};

UCLASS()
class EXPPROJECT_API UMyUserAccountWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// Handle Login:
	UFUNCTION(BlueprintCallable, Category = "AccountClient")
	void HandleLogin(FString username, FString password);

	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = "AccountClient")
		void NotifyLoginSuccess(const FLoginResponse& fLoginResponse);

	UFUNCTION(BlueprintImplementableEvent, Category = "AccountClient")
		void NotifyLoginFail();

	// Handle Register:
	UFUNCTION(BlueprintCallable, Category = "AccountClient")
	void HandleRegister(FString email, FString firstname, FString lastname, FString mobileno, FString password, FString username);

	void OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = "AccountClient")
		void NotifyRegisterSuccess(const FRegisterResponse& fRegisterResponse);

	UFUNCTION(BlueprintImplementableEvent, Category = "AccountClient")
		void NotifyFalseSuccess(const FRegisterResponse& fRegisterResponse);

	UFUNCTION(BlueprintImplementableEvent, Category = "AccountClient")
		void NotifyRegisterFail();

	// Get user characters
	UFUNCTION(BlueprintCallable, Category = "AccountClient")
		void GetAccountCharacters(FString AccessToken);

	void OnGetCharacterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = "AccountClient")
		void NotifyGetCharacterSuccess(const FAccountCharacterResponse& fAccountCharacterResponse);

	UFUNCTION(BlueprintImplementableEvent, Category = "AccountClient")
		void NotifyGetCharacterFail();

	// Create user characters
	UFUNCTION(BlueprintCallable, Category = "AccountClient")
		void CreateNewCharacter(FString AccessToken, FString name, TMap<FString, FString> appearanceInfo);

	void OnCreateNewCharacterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintImplementableEvent, Category = "AccountClient")
		void NotifyCreateNewCharacterSuccess(const FAccountCharacterBase& fAccountCharacterBase);

	UFUNCTION(BlueprintImplementableEvent, Category = "AccountClient")
		void NotifyCreateNewCharacterFail();


private:
	FHttpModule* Http;
	FString LoginApiPath;
	FString RegisterApiPath;
	FString GetAccountCharactersApiPath;
	FString CreateNewCharacterApiPath;

	void SetHeaders(TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request, FString AccessToken);
};
