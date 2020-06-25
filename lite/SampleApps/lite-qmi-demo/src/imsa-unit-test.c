#include "imsa.h"
#include <stdio.h>
#include <string.h>
void imsa_reg_ind_unpack_test()
{
    int rtn;
    uint8_t imsa_reg_unpack_input[] = {
        0x02, 0x01, 0x00, 0x22, 0x00, 0x07, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    unpack_imsa_SLQSRegisterIMSAIndication_t output;

    rtn = unpack_imsa_SLQSRegisterIMSAIndication(
                imsa_reg_unpack_input,
                sizeof(imsa_reg_unpack_input),
                &output
                );

    if(swi_uint256_get_bit (output.ParamPresenceMask, 2) && !(output.Tlvresult))
        printf("unpack_imsa_SLQSRegisterIMSAIndication failed %d\n", rtn);

}

void imsa_reg_ind_pack_test()
{
    uint8_t imsa_reg_null_param_pack_expected[] = {
        0x00, 0x01, 0x00, 0x22, 0x00, 0x00, 0x00
    };
    uint8_t imsa_reg_with_reg_status_pack_expected[] = {
        0x00, 0x02, 0x00, 0x22, 0x00, 0x04, 0x00, 0x10, 0x01, 0x00, 0x01
    };
    uint8_t imsa_reg_with_svc_status_pack_expected[] = {
        0x00, 0x03, 0x00, 0x22, 0x00, 0x04, 0x00, 0x11, 0x01, 0x00, 0x01
    };
    uint8_t imsa_reg_with_rat_handover_pack_expected[] = {
        0x00, 0x04, 0x00, 0x22, 0x00, 0x04, 0x00, 0x12, 0x01, 0x00, 0x01
    };
    uint8_t imsa_reg_with_pdp_status_pack_expected[] = {
        0x00, 0x05, 0x00, 0x22, 0x00, 0x04, 0x00, 0x13, 0x01, 0x00, 0x01
    };
    uint8_t imsa_reg_all_pack_expected[] = {
        0x00, 0x06, 0x00, 0x22, 0x00, 0x10, 0x00, 0x10, 0x01, 0x00, 0x01, 0x11, 0x01, 0x00, 0x01, 0x12, 0x01, 0x00, 0x01, 0x13, 0x01, 0x00, 0x01
    };
    int rtn;
    pack_imsa_SLQSRegisterIMSAIndication_t param;
    uint8_t req[2048];
    uint16_t len = sizeof(req);
    pack_qmi_t ctx;

    ctx.xid = 1;
    memset(&param, 0, sizeof(param));
    rtn = pack_imsa_SLQSRegisterIMSAIndication(&ctx, req, &len, &param);
    if (
            (rtn != 0)
            || (len != sizeof(imsa_reg_null_param_pack_expected))
            || memcmp(req, imsa_reg_null_param_pack_expected,
                sizeof(imsa_reg_null_param_pack_expected))
       )
        printf("pack_imsa_SLQSRegisterIMSAIndication null param case not expected: rtn %d, len %d\n",
                rtn, len);

    ctx.xid = 2;
    len = sizeof(req);
    memset(&param, 0, sizeof(param));
    param.RegStatusConfig = 1;
    param.has_RegStatusConfig = 1;
    rtn = pack_imsa_SLQSRegisterIMSAIndication(&ctx, req, &len, &param);
    if (
            (rtn != 0)
            || (len != sizeof(imsa_reg_with_reg_status_pack_expected))
            || memcmp(req, imsa_reg_with_reg_status_pack_expected,
                sizeof(imsa_reg_with_reg_status_pack_expected))
       )
        printf("pack_imsa_SLQSRegisterIMSAIndication reg status case not expected: rtn %d, len %d\n",
                rtn, len);

    ctx.xid = 3;
    len = sizeof(req);
    memset(&param, 0, sizeof(param));
    param.ServiceStatusConfig = 1;
    param.has_ServiceStatusConfig = 1;
    rtn = pack_imsa_SLQSRegisterIMSAIndication(&ctx, req, &len, &param);
    if (
            (rtn != 0)
            || (len != sizeof(imsa_reg_with_svc_status_pack_expected))
            || memcmp(req, imsa_reg_with_svc_status_pack_expected,
                sizeof(imsa_reg_with_svc_status_pack_expected))
       )
        printf("pack_imsa_SLQSRegisterIMSAIndication service status case not expected: rtn %d, len %d\n",
                rtn, len);

    ctx.xid = 4;
    len = sizeof(req);
    memset(&param, 0, sizeof(param));
    param.RatHandoverStatusConfig = 1;
    param.has_RatHandoverStatusConfig = 1;
    rtn = pack_imsa_SLQSRegisterIMSAIndication(&ctx, req, &len, &param);
    if (
            (rtn != 0)
            || (len != sizeof(imsa_reg_with_rat_handover_pack_expected))
            || memcmp(req, imsa_reg_with_rat_handover_pack_expected,
                sizeof(imsa_reg_with_rat_handover_pack_expected))
       )
        printf("pack_imsa_SLQSRegisterIMSAIndication service status case not expected: rtn %d, len %d\n",
                rtn, len);

    ctx.xid = 5;
    len = sizeof(req);
    memset(&param, 0, sizeof(param));
    param.PdpStatusConfig = 1;
    param.has_PdpStatusConfig = 1;
    rtn = pack_imsa_SLQSRegisterIMSAIndication(&ctx, req, &len, &param);
    if (
            (rtn != 0)
            || (len != sizeof(imsa_reg_with_pdp_status_pack_expected))
            || memcmp(req, imsa_reg_with_pdp_status_pack_expected,
                sizeof(imsa_reg_with_pdp_status_pack_expected))
       )
        printf("pack_imsa_SLQSRegisterIMSAIndication service status case not expected: rtn %d, len %d\n",
                rtn, len);

    ctx.xid = 6;
    len = sizeof(req);
    memset(&param, 0, sizeof(param));
    param.RegStatusConfig = 1;
    param.has_RegStatusConfig = 1;
    param.ServiceStatusConfig = 1;
    param.has_ServiceStatusConfig = 1;
    param.RatHandoverStatusConfig = 1;
    param.has_RatHandoverStatusConfig = 1;
    param.PdpStatusConfig = 1;
    param.has_PdpStatusConfig = 1;
    rtn = pack_imsa_SLQSRegisterIMSAIndication(&ctx, req, &len, &param);
    if (
            (rtn != 0)
            || (len != sizeof(imsa_reg_all_pack_expected))
            || memcmp(req, imsa_reg_all_pack_expected,
                sizeof(imsa_reg_all_pack_expected))
       )
        printf("pack_imsa_SLQSRegisterIMSAIndication service status case not expected: rtn %d, len %d\n",
                rtn, len);
}

void imsa_reg_status_pack_test()
{
    uint8_t expected[] = {
        0x00, 0x09, 0x00, 0x20, 0x00, 0x00, 0x00
    };
    int rtn;
    uint8_t req[2048];
    uint16_t len = sizeof(req);
    pack_qmi_t ctx;

    ctx.xid = 9;
    rtn = pack_imsa_SLQSGetIMSARegStatus(&ctx, req, &len);
    if (
            (rtn != 0)
            || (len != sizeof(expected))
            || memcmp(req, expected,
                sizeof(expected))
       )
        printf("pack_imsa_SLQSGetIMSARegStatus not expected: rtn %d, len %d\n",
                rtn, len);

}

void imsa_reg_status_unpack_test()
{
    int rtn;
    uint8_t input[] = {
        0x02, 0x09, 0x00, 0x20, 0x00, 0x12, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x01, 0x12, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00
    };
    unpack_imsa_SLQSGetIMSARegStatus_t output;
    unpack_imsa_SLQSGetIMSARegStatus_t expected = {1, ~0, 2, {{0}}};

    rtn = unpack_imsa_SLQSGetIMSARegStatus(
                input,
                sizeof(input),
                &output);

    if (
            ( 0 != rtn ) ||
            ( output.ImsRegStatus != expected.ImsRegStatus ) ||
            ( output.ImsRegErrCode != expected.ImsRegErrCode ) ||
            ( output.NewImsRegStatus != expected.NewImsRegStatus )
       )
        printf("unpack_imsa_SLQSGetIMSARegStatus failed %d\n", rtn);

}

void imsa_svc_status_pack_test()
{
    uint8_t expected[] = {
        0x00, 0x0A, 0x00, 0x21, 0x00, 0x00, 0x00
    };
    int rtn;
    uint8_t req[2048];
    uint16_t len = sizeof(req);
    pack_qmi_t ctx;

    ctx.xid = 10;
    rtn = pack_imsa_SLQSGetIMSAServiceStatus(&ctx, req, &len);
    if (
            (rtn != 0)
            || (len != sizeof(expected))
            || memcmp(req, expected,
                sizeof(expected))
       )
        printf("pack_imsa_SLQSGetIMSAServiceStatus not expected: rtn %d, len %d\n",
                rtn, len);

}

void imsa_svc_status_unpack_test()
{
    int rtn;
    uint8_t input[] = {
        0x02, 0x0A, 0x00, 0x21, 0x00, 0x38, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x10, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 
        0x11, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x12, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x13, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 
        0x14, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 
        0x15, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 
        0x16, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    unpack_imsa_SLQSGetIMSAServiceStatus_t output;
    unpack_imsa_SLQSGetIMSAServiceStatus_t expected = {2, 0, 0, 1, 1, 1, 0, ~0, ~0, ~0, {{0}}};

    rtn = unpack_imsa_SLQSGetIMSAServiceStatus(
                input,
                sizeof(input),
                &output);

    if (
            ( 0 != rtn ) ||
            ( output.SmsServiceStatus != expected.SmsServiceStatus ) ||
            ( output.VoipServiceStatus != expected.VoipServiceStatus ) ||
            ( output.VtServiceStatus != expected.VtServiceStatus ) ||
            ( output.SmsServiceRat != expected.SmsServiceRat ) ||
            ( output.VoipServiceRat != expected.VoipServiceRat ) ||
            ( output.VtServiceRat != expected.VtServiceRat ) ||
            ( output.UtServiceStatus != expected.UtServiceStatus ) ||
            ( output.UtServiceRat != expected.UtServiceRat ) ||
            ( output.VsServiceStatus != expected.VsServiceStatus ) ||
            ( output.VsServiceRat != expected.VsServiceRat )
       )
        printf("unpack_imsa_SLQSGetIMSAServiceStatus failed %d\n", rtn);

}

void imsa_unit_test()
{
    imsa_reg_ind_pack_test();
    imsa_reg_ind_unpack_test();

    imsa_reg_status_pack_test();
    imsa_reg_status_unpack_test();

    imsa_svc_status_pack_test();
    imsa_svc_status_unpack_test();
}
