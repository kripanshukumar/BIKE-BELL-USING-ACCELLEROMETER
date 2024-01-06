void PrintData(const char* Data,uint8_t NumBytes)
{
    for(uint8_t i=0;i<NumBytes;i++)
      Serial.print(Data[i]); 
      Serial.println();  
}

bool ValidWavData(WavHeader_Struct* Wav)
{
  
  if(memcmp(Wav->RIFFSectionID,"RIFF",4)!=0) 
  {    
    Serial.print("Invlaid data - Not RIFF format");
    return false;        
  }
  if(memcmp(Wav->RiffFormat,"WAVE",4)!=0)
  {
    Serial.print("Invlaid data - Not Wave file");
    return false;           
  }
  if(memcmp(Wav->FormatSectionID,"fmt",3)!=0) 
  {
    Serial.print("Invlaid data - No format section found");
    return false;       
  }
  if(memcmp(Wav->DataSectionID,"data",4)!=0) 
  {
    Serial.print("Invlaid data - data section not found");
    return false;      
  }
  if(Wav->FormatID!=1) 
  {
    Serial.print("Invlaid data - format Id must be 1");
    return false;                          
  }
  if(Wav->FormatSize!=16) 
  {
    Serial.print("Invlaid data - format section size must be 16.");
    return false;                          
  }
  if((Wav->NumChannels!=1)&(Wav->NumChannels!=2))
  {
    Serial.print("Invlaid data - only mono or stereo permitted.");
    return false;   
  }
  if(Wav->SampleRate>48000) 
  {
    Serial.print("Invlaid data - Sample rate cannot be greater than 48000");
    return false;                       
  }
  if((Wav->BitsPerSample!=8)& (Wav->BitsPerSample!=16)) 
  {
    Serial.print("Invlaid data - Only 8 or 16 bits per sample permitted.");
    return false;                        
  }
  return true;
}


void DumpWAVHeader(WavHeader_Struct* Wav)
{
  if(memcmp(Wav->RIFFSectionID,"RIFF",4)!=0)
  {
    Serial.print("Not a RIFF format file - ");    
    //PrintData(Wav->RIFFSectionID,4);
    return;
  } 
  if(memcmp(Wav->RiffFormat,"WAVE",4)!=0)
  {
    Serial.print("Not a WAVE file - ");  
    //PrintData(Wav->RiffFormat,4);  
    return;
  }  
  if(memcmp(Wav->FormatSectionID,"fmt",3)!=0)
  {
    Serial.print("fmt ID not present - ");
    //PrintData(Wav->FormatSectionID,3);      
    return;
  } 
  if(memcmp(Wav->DataSectionID,"data",4)!=0)
  {
    Serial.print("data ID not present - "); 
    //PrintData(Wav->DataSectionID,4);
    return;
  }  
  // All looks good, dump the data
  Serial.print("Total size :");Serial.println(Wav->Size);
  Serial.print("Format section size :");Serial.println(Wav->FormatSize);
  Serial.print("Wave format :");Serial.println(Wav->FormatID);
  Serial.print("Channels :");Serial.println(Wav->NumChannels);
  Serial.print("Sample Rate :");Serial.println(Wav->SampleRate);
  Serial.print("Byte Rate :");Serial.println(Wav->ByteRate);
  Serial.print("Block Align :");Serial.println(Wav->BlockAlign);
  Serial.print("Bits Per Sample :");Serial.println(Wav->BitsPerSample);
  Serial.print("Data Size :");Serial.println(Wav->DataSize);
//  i2s_config.sample_rate = Wav->SampleRate;
//  i2s_config.bits_per_sample = Wav->BitsPerSample;
//  i2s.channel_format = 
}
