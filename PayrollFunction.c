#include <stdio.h>
#include "PayrollAbstract.h"
// các giới hạn và tỉ lệ
#define CPP_LIMIT     1861.20
#define CPP_RATE      0.0495
#define CPP_EXEMPTION 3500.00
#define EI_LIMIT      760.50
#define EI_RATE       0.0195
#define LCP_LIMIT	  750	
// các mức chịu thuế liên bang
#define FED_TAX_BRACKET1 35595.00
#define FED_TAX_BRACKET2 71190.00
#define FED_TAX_BRACKET3 115739.00
// các mức chịu thuế tỉnh
#define PROV_TAX_BRACKET1 34010.00
#define PROV_TAX_BRACKET2 68020.00
// các tỉ lệ chịu thuế liên bang
#define FED_TAX_RATE1 0.16
#define FED_TAX_RATE2 0.22
#define FED_TAX_RATE3 0.26
#define FED_TAX_RATE4 0.29
// các tỉ lệ chịu thuế liên bang
#define PROV_TAX_RATE1 0.0605
#define PROV_TAX_RATE2 0.0915
#define PROV_TAX_RATE3 0.1116
// các mức phí bảo hiểm 
#define HEALTH_PREMIUM_BRACKET1 20000.00
#define HEALTH_PREMIUM_BRACKET2 36000.00
#define HEALTH_PREMIUM_BRACKET3 48000.00
#define HEALTH_PREMIUM_BRACKET4	72000.00
#define HEALTH_PREMIUM_BRACKET5 200000.00
// các tỷ lệ phải chịu của bảo hiểm
#define HEALTH_PREMIUM_RATE1 0.06
#define HEALTH_PREMIUM_RATE2 0.25
// các giới hạn cơ của của bảo hiểm sức khỏe
#define BASE_LIMIT1 0
#define BASE_LIMIT2 300
#define BASE_LIMIT3 450
#define BASE_LIMIT4 600
#define BASE_LIMIT5 750
// các giới hạn phí bảo hiểm
#define PREMIUM_LIMIT1 300
#define PREMIUM_LIMIT2 450
#define PREMIUM_LIMIT3 600
#define PREMIUM_LIMIT4 750
#define PREMIUM_LIMIT5 900
// hàm tính cpp theo kỳ
double caculate_cpp(double income, int pay_periods, double ytd_cpp) {
	double cpp = CPP_RATE * (income - CPP_EXEMPTION / pay_periods); // cpp = tỉ lệ cpp là 4.95% nhân với thu nhập 1 kỳ
	//  kiểm tra xem tổng cpp (cả cpp hiện tại và cpp đã đóng) có vượt quá giới hạn hay không
	cpp = (cpp + ytd_cpp > CPP_LIMIT) ? (CPP_LIMIT - ytd_cpp) : cpp; // điều chỉnh để không vượt giới hạn
	// nếu cpp âm thì trả về là 0, cpp không được âm
	cpp = (cpp < 0) ? 0 : cpp;
	return cpp;
}
// hàm tính ei theo kỳ
double caculate_ei(double income, double ytd_ei) {
	double ei = EI_RATE * income; // ei = EI_RATE là 1.95% nhân với thu nhập hiện tại
	// kiểm tra xem tổng EI (gồm EI hiện tại và ei_ytd đã đóng) có vượt quá giới hạn không
	ei = (ei + ytd_ei > EI_LIMIT) ? (EI_LIMIT - ytd_ei) : ei; // điều chỉnh để không vượt giới hạn
	// nếu ei âm thì trả về là 0
	ei = (ei < 0) ? 0 : ei;
	return ei;
}
// hàm tính thuế liên bang
double calculate_federal_tax(double income, double rpp, double union_dues, double federal_credits, double pay_periods, double lssp, double cpp, double ei, double ad) {
	// tính thu nhập chịu thuế
	double taxable_income = pay_periods * (income - rpp - union_dues) - ad;
	// tính thuế liên bang dựa trên các bậc thuế khác nhau
	if (taxable_income <= FED_TAX_BRACKET1) {
		taxable_income = taxable_income * FED_TAX_RATE1;
	} else if (taxable_income <= FED_TAX_BRACKET2) {
		taxable_income = taxable_income * FED_TAX_RATE2 - 2136;
	} else if (taxable_income <= FED_TAX_BRACKET3) {
		taxable_income = taxable_income * FED_TAX_RATE3 - 4983;
	} else {
		taxable_income = taxable_income * FED_TAX_RATE4 - 8455;
	}
	// tính đóng góp CPP và so sánh giới hạn
	cpp = cpp * pay_periods;
	cpp = (cpp >= CPP_LIMIT) ? CPP_LIMIT : cpp;
	// tính đóng góp EI và so sánh giới hạn
	ei = ei * pay_periods;
	ei = (ei >= EI_LIMIT) ? EI_LIMIT : ei;
	// tính LCP, so sánh max là LCP_LIMIT là 750 lấy nhỏ hơn
	double LCP = 0.15 * lssp;
	LCP = (LCP >= LCP_LIMIT) ? LCP_LIMIT : LCP;
	// tính thuế cuối cùng
	double tax = taxable_income - 0.16*(federal_credits + cpp + ei);
	tax = (tax - LCP) / pay_periods;
	// thuế không âm, nếu âm trả về 0
	tax = (tax < 0) ? 0 : tax;
	return tax;
}

// hàm tính thuế tỉnh Ontario
double caculate_provincial_tax(double income, double rpp, double union_dues, double pay_periods, double provincial_credits, double dependants, double lssp, double cpp, double ei, double ad) {
	// tính thu nhập chịu thuế
	double taxable_income = pay_periods * (income - rpp - union_dues) - ad;
	double A = taxable_income; // đặt thu nhập chịu thuế thành A để sử dụng cho tính thuế bảo hiểm sức khỏe
	// tính thuế dựa trên mức thuế
	if (taxable_income <= PROV_TAX_BRACKET1) {
		taxable_income = taxable_income * PROV_TAX_RATE1;
	} else if (taxable_income <= PROV_TAX_BRACKET2) {
		taxable_income = taxable_income * PROV_TAX_RATE2 - 1054;
	} else {
		taxable_income = taxable_income * PROV_TAX_RATE3 - 2422;
	}
	// tính đóng góp CPP và so sánh giới hạn
	cpp = cpp * pay_periods;
	cpp = (cpp >= CPP_LIMIT) ? CPP_LIMIT : cpp;
	// tính đóng góp EI và so sánh giới hạn
	ei = ei * pay_periods;
	ei = (ei >= EI_LIMIT) ? EI_LIMIT : ei;
	// tính thu nhập chịu thuế sau khi khấu trừ
	taxable_income = taxable_income - 0.0605*(provincial_credits + cpp + ei);
	// tính thuế phụ
	double surtax_a = (taxable_income - 3929) * 0.2;
	double surtax_b = (taxable_income - 4957) * 0.36;
	surtax_a = (surtax_a < 0) ? 0 : surtax_a; // nếu thuế phụ < 0 thì trả về giá trị là 0
	surtax_b = (surtax_b < 0) ? 0 : surtax_b;
	double total_surtax = surtax_a + surtax_b; // tính tổng các thuế phụ phải trả
	
	double tax_reduction = 2 * (190 + 350*dependants) - (taxable_income + total_surtax); // tính giảm thuế tỉnh
	tax_reduction = (tax_reduction >= taxable_income + total_surtax) ? (taxable_income + total_surtax) : tax_reduction;
	tax_reduction = (tax_reduction < 0) ? 0 : tax_reduction; // nếu thuế nhỏ hơn 0 thì trả về giá trị là 0
	// tính LCP, so sánh max là LCP_LIMIT là 750 lấy nhỏ hơn
	double LCP = 0.15 * lssp;
	LCP = (LCP >= LCP_LIMIT) ? LCP_LIMIT : LCP;

	double v2 = 0; // khai báo v2 là tổng thuế bảo hiểm sức khỏe, nếu nhỏ hơn HEALTH_PREMIUM_BRACKET1 là 20000 thì thuế là 0
	// tính thuế theo mức bảo hiếm sức khỏe
	if (A > HEALTH_PREMIUM_BRACKET1 && A <= HEALTH_PREMIUM_BRACKET2) {
		v2 = (A - HEALTH_PREMIUM_BRACKET1) * HEALTH_PREMIUM_RATE1 + BASE_LIMIT1;
		v2 = (v2 >= PREMIUM_LIMIT1) ? PREMIUM_LIMIT1 : v2;
	} else if (A > HEALTH_PREMIUM_BRACKET2 && A <= HEALTH_PREMIUM_BRACKET3) {
		v2 = (A - HEALTH_PREMIUM_BRACKET2) * HEALTH_PREMIUM_RATE1 + BASE_LIMIT2;
		v2 = (v2 >= PREMIUM_LIMIT2) ? PREMIUM_LIMIT2 : v2;
	} else if (A > HEALTH_PREMIUM_BRACKET3 && A <= HEALTH_PREMIUM_BRACKET4) {
		v2 = (A - HEALTH_PREMIUM_BRACKET3) * HEALTH_PREMIUM_RATE2 + BASE_LIMIT3;
		v2 = (v2 >= PREMIUM_LIMIT3) ? PREMIUM_LIMIT3 : v2;
	} else if (A > HEALTH_PREMIUM_BRACKET4 && A <= HEALTH_PREMIUM_BRACKET5) {
		v2 = (A - HEALTH_PREMIUM_BRACKET4) * HEALTH_PREMIUM_RATE2 + BASE_LIMIT4;
		v2 = (v2 >= PREMIUM_LIMIT4) ? PREMIUM_LIMIT4 : v2;
	} else {
		v2 = (A - HEALTH_PREMIUM_BRACKET5) * HEALTH_PREMIUM_RATE2 + BASE_LIMIT5;
		v2 = (v2 >= PREMIUM_LIMIT5) ? PREMIUM_LIMIT5 : v2;
	}
	
	// tính chịu thuế tỉnh cuối cùng chia cho số kỳ
	taxable_income = (taxable_income + total_surtax + v2 - tax_reduction - LCP) / pay_periods;
	return taxable_income < 0 ? 0 : taxable_income; // nếu chịu thuế tỉnh nhỏ hơn 0 thì trả về 0
}
// hàm tính tổng các khoản khấu trừ
double calculate_total_deductions(double rpp, double union_dues, double federal_tax, double provincial_tax, double cpp, double ei) {
	return federal_tax + provincial_tax + cpp + ei + rpp + union_dues;
}
// hàm tính thu nhập ròng
double calculate_net_income(double income, double total_deductions) {
	return income - total_deductions;
}