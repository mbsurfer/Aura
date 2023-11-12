


#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"


FAuraAttributeInfo UAttributeInfo::GetAttributeInfoByTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
    for (const FAuraAttributeInfo& Info : AttributeInformation)
    {
        if (Info.AttributeTag == AttributeTag)
        {
            return Info;
        }
    }

    if (bLogNotFound)
    {
        UE_LOG(LogTemp, Error, TEXT("Info for AttributeTag [%s] not found."), *AttributeTag.ToString());
    }

    return FAuraAttributeInfo();
}
