#ifndef PAYROLLABSTRACT_H
#define PAYROLLABSTRACT_H

// khởi tạo các hàm abstract
double caculate_cpp(double income, int pay_periods, double ytd_cpp);
// hàm tính EI theo kỳ
double caculate_ei(double income, double ytd_ei);
// hàm tính thuế liên bang theo kỳ
double calculate_federal_tax(double income, double rpp, double union_dues, double federal_credits, double pay_periods, double lssp, double cpp, double ei, double ad);
// hàm tính thuế tỉnh theo kỳ
double caculate_provincial_tax(double income, double rpp, double union_dues, double pay_periods, double provincial_credits, double dependants, double lssp, double cpp, double ei, double ad);
// hàm tính tổng thu thuế khấu trừ
double calculate_total_deductions(double rpp, double union_dues, double federal_tax, double provincial_tax, double cpp, double ei);
// hàm tính thu nhập ròng
double calculate_net_income(double income, double total_deductions);

#endif