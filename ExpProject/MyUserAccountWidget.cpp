// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserAccountWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"

void UMyUserAccountWidget::HandleLogin(FString username, FString password)
{
	LoginApiPath = "http://localhost:8080/cskaa/user/login";

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UMyUserAccountWidget::OnLoginResponseReceived);

	TSharedPtr<FJsonObject> params = MakeShareable(new FJsonObject);
	params->SetStringField(TEXT("userName"), username);
	params->SetStringField(TEXT("password"), password);

	FString paramsString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&paramsString);
	FJsonSerializer::Serialize(params.ToSharedRef(), JsonWriter);

	Request->SetURL(LoginApiPath);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Accept", TEXT("application/json"));

	Request->SetContentAsString(paramsString);
	Request->ProcessRequest();
}

void UMyUserAccountWidget::OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bool success = false;
	UE_LOG(LogTemp, Warning, TEXT("Your message: %s"), *Response->GetContentAsString());
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			
			if (JsonObject->HasField("status"))
			{
				FLoginResponse loginResponse;
				loginResponse.Username = JsonObject->GetStringField("userName");
				JsonObject->TryGetStringArrayField("roles", loginResponse.Roles);
				loginResponse.AccessToken = JsonObject->GetStringField("token");
		
				bool retu = JsonObject->GetBoolField("status");
				if (retu)
				{
					success = true;
					NotifyLoginSuccess(loginResponse);
				}
				//if (loginResponse.AccessToken.Equals("access_token"))
				//{
				//	success = true;
				//	NotifyLoginSuccess(loginResponse);
				//}
			}
		}
	}
	if (success == false)
	{
		NotifyLoginFail();
	}
}

void UMyUserAccountWidget::HandleRegister(FString email, FString firstname, FString lastname, FString mobileno, FString password, FString username)
{	
	RegisterApiPath = "http://localhost:8080/cskaa/user/register";

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UMyUserAccountWidget::OnRegisterResponseReceived);

	TSharedPtr<FJsonObject> params = MakeShareable(new FJsonObject);
	params->SetStringField(TEXT("userName"), username);
	params->SetStringField(TEXT("password"), password);
	params->SetStringField(TEXT("emailId"), email);
	params->SetStringField(TEXT("lastName"), "Rathore");
	params->SetStringField(TEXT("firstName"), "Ayush");
	params->SetStringField(TEXT("mobileNo"), "9818536716");

	FString paramsString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&paramsString);
	FJsonSerializer::Serialize(params.ToSharedRef(), JsonWriter);

	Request->SetURL(RegisterApiPath);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Accept", TEXT("application/json"));

	UE_LOG(LogTemp, Warning, TEXT("Your message = %s"), *paramsString);
	Request->SetContentAsString(paramsString);
	Request->ProcessRequest();
}

void UMyUserAccountWidget::OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bool success = false;
	UE_LOG(LogTemp, Warning, TEXT("Your message = %s"), *Response->GetContentAsString());
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
	
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (JsonObject->HasField("status"))
			{
				FRegisterResponse registerResponse;

				bool ret = JsonObject->GetBoolField("status");
				if (ret)
				{
					success = true;
					NotifyRegisterSuccess(registerResponse);
				}
			}
		}
	}
	if (success == false)
	{
		NotifyRegisterFail();
	}
}

void UMyUserAccountWidget::GetAccountCharacters(FString AccessToken)
{
	GetAccountCharactersApiPath = "http://localhost:8081/player/account-characters";

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UMyUserAccountWidget::OnGetCharacterResponse);

	Request->SetURL(GetAccountCharactersApiPath);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Accept", TEXT("application/json"));
	FString TokenString = FString(TEXT("Bearer ") + AccessToken);
	Request->SetHeader("Authorization", TokenString);
	Request->ProcessRequest();
}

void UMyUserAccountWidget::OnGetCharacterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bool success = false;
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (JsonObject->HasField("accountCharacters"))
			{
				TArray<TSharedPtr<FJsonValue>> Rows = JsonObject->GetArrayField("accountCharacters");
				FAccountCharacterResponse Response;
				success = true;
				for (int i = 0; i < Rows.Num(); i++)
				{
					FAccountCharacterBase BaseCharacter;
					TSharedPtr<FJsonObject> tempRow = Rows[i]->AsObject();

					BaseCharacter.AccountName = tempRow->GetStringField("accountName");
					BaseCharacter.Name = tempRow->GetStringField("name");
					BaseCharacter.Xp = tempRow->GetNumberField("xp");

					TMap<FString, FString> appearanceInfo;
					TSharedPtr<FJsonObject> appearanceItems = tempRow->GetObjectField("appearanceInfo");
					
					// Iterate over Json Values
					for (auto currJsonValue = appearanceItems->Values.CreateConstIterator(); currJsonValue; ++currJsonValue)
					{
						appearanceInfo.Add(currJsonValue->Key, appearanceItems->GetStringField(currJsonValue->Key));
					}

					BaseCharacter.AppearanceInfo = appearanceInfo;
					Response.CharacterList.Add(BaseCharacter);
				}

				NotifyGetCharacterSuccess(Response);
			}
		}
	}

	if (success == false)
	{
		NotifyGetCharacterFail();
	}
}

void UMyUserAccountWidget::CreateNewCharacter(FString AccessToken, FString name, TMap<FString, FString> appearanceInfo)
{
	CreateNewCharacterApiPath = "http://localhost:8081/player/create-character";

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UMyUserAccountWidget::OnCreateNewCharacterResponse);

	TSharedPtr<FJsonObject> params = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> appearanceparams = MakeShareable(new FJsonObject);
	for (auto& Elem : appearanceInfo)
	{
		appearanceparams->SetStringField(Elem.Key, Elem.Value);
	}
	params->SetObjectField(TEXT("appearanceInfo"), appearanceparams);
	params->SetStringField(TEXT("name"), name);

	FString paramsString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&paramsString);
	FJsonSerializer::Serialize(params.ToSharedRef(), JsonWriter);

	Request->SetURL(CreateNewCharacterApiPath);
	Request->SetVerb("POST");
	SetHeaders(Request, AccessToken);

	Request->SetContentAsString(paramsString);
	Request->ProcessRequest();
}

void UMyUserAccountWidget::OnCreateNewCharacterResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bool success = false;

	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (JsonObject->HasField("name"))
			{
				FAccountCharacterBase CharacterBase;

				CharacterBase.AccountName = JsonObject->GetStringField("accountName");
				CharacterBase.Name = JsonObject->GetStringField("name");
				CharacterBase.Xp = JsonObject->GetNumberField("xp");

				if (!CharacterBase.AccountName.Equals(""))
				{
					NotifyCreateNewCharacterSuccess(CharacterBase);
				}
			}
		}
	}
	if (success == false)
	{
		NotifyCreateNewCharacterFail();
	}
}

void UMyUserAccountWidget::SetHeaders(TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request, FString AccessToken)
{
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Accept", TEXT("application/json"));
	if (AccessToken != "")
	{
		FString TokenString = FString(TEXT("Bearer ") + AccessToken);
		Request->SetHeader("Authorization", TokenString);
	}
}
