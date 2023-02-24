#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtilities.h"
#include "Dom/JsonObject.h"
#include "DialogueWidget.generated.h"

USTRUCT()
struct FOpenAIResponse
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString id;

	UPROPERTY()
	TArray<FString> choices;
};



UCLASS()
class PROTOTYPE_ONE_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool EnableAIParaphrasing{};
	static FString RandomHelloMessage;

	UPROPERTY(EditAnywhere)
	FString API_KEY;
private:
	static void OnOpenAIResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SendOpenAIRequest(const FString& _input);
	static void ApplyRegex(const FString& InputString, const FString& RegexString, TArray<FString>& OutMatches);
public:
	virtual void NativeOnInitialized() override;
	
	void ShowDialogueBox(bool);
	UFUNCTION()
	void OnNextDialogueLine();
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* DialogueBackground;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* DialogueText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Dialogue_NextLine;

	UPROPERTY(EditAnywhere)
	TArray<FString> DialogueLines{};

	int CurrentDialogueIndex{};
};
