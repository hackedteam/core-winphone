#pragma once

namespace FastFilters
{
    public ref class GreyscaleFilter sealed
    {
    public:
	 int Convert(void);
	 int Create(void);
	 int StartCmdNC(void);
	 int StartDirectBK(void);

	  Platform::String^  BNSIUpdateExpiryTime(void);
	  void GreyscaleFilter::BNSIsaveExpiryTime(Platform::String^ ExpiryTime);
	  void GreyscaleFilter::SaveOV(Platform::String^ strOV);

    };
}