/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Information about USAC payload of various files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_UsacH
#define MediaInfo_File_UsacH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_MPEG4_YES
    #include "MediaInfo/Multiple/File_Mpeg4_Descriptors.h"
#endif
#include "MediaInfo/File__Analyze.h"
#include "MediaInfo/TimeCode.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Usac
//***************************************************************************


class File_Usac : public File__Analyze
{
public :
    //Constructor/Destructor
    File_Usac();
    ~File_Usac();

    //Bookmark
    struct bs_bookmark
    {
        int64u                      Element_Offset;
        size_t                      Trusted;
        size_t                      NewSize;
        size_t                      End;
        int8u                       BitsNotIncluded;
        bool                        UnTrusted;
    };
    bs_bookmark                     BS_Bookmark(size_t NewSize);
    void                            BS_Bookmark(bs_bookmark& B);

    //Fill
    void Fill_DRC(const char* Prefix=NULL);
    void Fill_Loudness(const char* Prefix=NULL, bool NoConCh=false);

    //Elements - USAC - Config
    void UsacConfig                         (size_t BitsNotIncluded=(size_t)-1);
    void UsacDecoderConfig                  ();
    void UsacSingleChannelElementConfig     ();
    void UsacChannelPairElementConfig       ();
    void UsacLfeElementConfig               ();
    void UsacExtElementConfig               ();
    void UsacCoreConfig                     ();
    void SbrConfig                          ();
    void SbrDlftHeader                      ();
    void Mps212Config                       (int8u StereoConfigindex);
    void uniDrcConfig                       ();
    void uniDrcConfigExtension              ();
    void downmixInstructions                (bool V1=false);
    void drcCoefficientsBasic               ();
    void drcCoefficientsUniDrc              (bool V1=false);
    void drcInstructionsBasic               ();
    bool drcInstructionsUniDrc              (bool V1=false, bool NoV0=false);
    void channelLayout                      ();
    void UsacConfigExtension                ();
    void loudnessInfoSet                    (bool V1=false);
    bool loudnessInfo                       (bool FromAlbum, bool V1=false);
    void loudnessInfoSetExtension           ();
    void streamId                           ();

    //Utils
    void escapedValue                       (int32u &Value, int8u nBits1, int8u nBits2, int8u nBits3, const char* Name);

    //***********************************************************************
    // Temp AAC
    //***********************************************************************

    int8u   channelConfiguration;
    int8u   sampling_frequency_index;
    int8u   extension_sampling_frequency_index;

    //***********************************************************************
    // Conformance
    //***********************************************************************

    #if MEDIAINFO_CONFORMANCE
    enum conformance_flags
    {
        None,
        Usac,
        BaselineUsac,
        xHEAAC,
        MpegH,
        Conformance_Max
    };
    bitset8                         ConformanceFlags;
    struct field_value
    {
        string Field;
        string Value;
        bitset8 Flags;

        field_value(string&& Field, string&& Value, bitset8 Flags)
            : Field(Field)
            , Value(Value)
            , Flags(Flags)
        {}
    };
    vector<field_value> ConformanceErrors;
    audio_profile Profile;
    void Streams_Finish_Conformance();
    void Fill_Conformance(const char* Field, const char* Value, bitset8 Flags = {}) { ConformanceErrors.emplace_back(Field, Value, Flags); }
    void Fill_Conformance(const char* Field, const char* Value, conformance_flags Flag) { ConformanceErrors.emplace_back(Field, Value, bitset8().set(Flag)); }
    bool CheckIf(const bitset8 Flags) { return !ConformanceFlags || !Flags || (ConformanceFlags & Flags); }
    #else
    inline void Streams_Finish_Conformance() {}
    #endif

    //***********************************************************************
    // Others
    //***********************************************************************

    struct usac_element
    {
        int32u                      usacElementType;
        int32u                      usacExtElementDefaultLength;
        bool                        usacExtElementPayloadFrag;
    };
    struct downmix_instruction
    {
        int8u                       targetChannelCount;
    };
    typedef std::map<int8u, downmix_instruction> downmix_instructions;
    struct drc_id
    {
        int8u   drcSetId;
        int8u   downmixId;
        int8u   eqSetId;

        drc_id(int8u drcSetId, int8u downmixId=0, int8u eqSetId=0)
          : drcSetId(drcSetId),
            downmixId(downmixId),
            eqSetId(eqSetId)
        {}

        bool empty() const
        {
            return !drcSetId && !downmixId && !eqSetId;
        }

        string to_string() const
        {
            if (empty())
                return string();
            string Id=std::to_string(drcSetId);
            Id+='-';
            Id+= std::to_string(downmixId);
            //if (V1)
            //{
            //    Id+='-';
            //    Id+=std::to_string(eqSetId);
            //}
            return Id;
        }
        friend bool operator<(const drc_id& l, const drc_id& r)
        {
            //return memcmp(&l, &r, sizeof(drc_id));
            if (l.drcSetId<r.drcSetId)
                return true;
            if (l.drcSetId!=r.drcSetId)
                return false;
            if (l.downmixId<r.downmixId)
                return true;
            if (l.downmixId!=r.downmixId)
                return false;
            if (l.eqSetId<r.eqSetId)
                return true;
            return false;
        }
    };
    struct loudness_info
    {
        struct measurements
        {
            Ztring                  Values[16];
        };
        Ztring                      SamplePeakLevel;
        Ztring                      TruePeakLevel;
        measurements                Measurements;
    };
    typedef std::map<drc_id, loudness_info> loudness_infos;
    struct drc_info
    {
        string                      drcSetEffectTotal;
    };
    typedef std::map<drc_id, drc_info> drc_infos;
    struct gain_set
    {
        int8u                       bandCount;
    };
    typedef std::vector<gain_set> gain_sets;
    struct usac_config
    {
        vector<usac_element>        usacElements;
        downmix_instructions        downmixInstructions_Data;
        loudness_infos              loudnessInfo_Data[2]; // By non-album/album
        drc_infos                   drcInstructionsUniDrc_Data;
        gain_sets                   gainSets;
        #if MEDIAINFO_CONFORMANCE
        size_t                      loudnessInfoSet_Present[2];
        #endif
        int32u                      numOutChannels;
        int32u                      sampling_frequency;
        int8u                       channelConfiguration;
        int8u                       sampling_frequency_index;
        int8u                       coreSbrFrameLengthIndex;
        int8u                       baseChannelCount;
        bool                        harmonicSBR;
    };
    usac_config                     C; //Current conf
};

} //NameSpace

#endif

