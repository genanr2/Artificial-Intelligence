#include "stdafx.h"
#include "b50.h"
#include "b50Set.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_DYNAMIC(CB50Set,CDaoRecordset)
CB50Set::CB50Set(CDaoDatabase*pdb):CDaoRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CB50Set)
	m_column1 = 0.0;
	m_column2 = _T("");
	m_column3 = 0.0;
	m_column4 = _T("");
	m_column5 = _T("");
	m_column6 = _T("");
	m_______1 = 0.0;
	m___________2 = 0.0;
	m_column7 = 0.0;
	m_column8 = _T("");
	m_column9 = _T("");
	m_column10 = _T("");
	m_column11 = _T("");
	m_column12 = _T("");
	m_column13 = 0.0;
	m_UF_WAL_TA = 0.0;
	m_UF_ED_WAL = 0.0;
	m_KOL_UHR__L = 0.0;
	m_KOL_UHR_FL = 0.0;
	m_column14 = _T("");
	m_PRIZNAK_OB = 0.0;
	m_column15 = 0.0;
	m_column16 = 0.0;
	m_KOD_SOOGU = 0.0;
	m_KOD_SOATO = _T("");
	m_KOD_KOPF = 0.0;
	m_KOD_KSF = 0.0;
	m_column17 = _T("");
	m_column18 = 0.0;
	m_column19 = _T("");
	m_column20 = _T("");
	m_column21 = (DATE)0;
	m_column22 = (DATE)0;
	m_column23 = (DATE)0;
	m_TIMECHG = 0.0;
	m_DATECHG = (DATE)0;
	m_U_FIRM = 0.0;
	m_nFields = 37;
	//}}AFX_FIELD_INIT
	m_nDefaultType = dbOpenTable;
}
CString CB50Set::GetDefaultDBName()
{
//	return _T("d:\\work\\ast\\access\\gko_access.mdb");
	return _T("D:\\Налоговые проверки\\Налоговые проверки.mdb");
}
//CString CB50Set::GetDefaultSQL(){return _T("[Auction]");}
CString CB50Set::GetDefaultSQL(){return _T("[Предприятие2]");}
void CB50Set::DoFieldExchange(CDaoFieldExchange* pFX)
{
/*
	pFX->SetFieldType(CDaoFieldExchange::outputColumn);
	DFX_DateTime(pFX, _T("[Date_auc]"), m_Date_auc);
	DFX_Long(pFX, _T("[Nv]"), m_Nv);
	DFX_DateTime(pFX, _T("[Pog_date]"), m_Pog_date);
	DFX_Short(pFX, _T("[Days_in_use]"), m_Days_in_use);
	DFX_Text(pFX, _T("[Transh]"), m_Transh);
	DFX_Double(pFX, _T("[Val_vip]"), m_Val_vip);
	DFX_Double(pFX, _T("[Val_not_conc]"), m_Val_not_conc);
	DFX_Double(pFX, _T("[Bid]"), m_Bid);
	DFX_Double(pFX, _T("[Obor_rub]"), m_Obor_rub);
	DFX_Double(pFX, _T("[Price_avg]"), m_Price_avg);
	DFX_Double(pFX, _T("[Price_min]"), m_Price_min);
	DFX_Double(pFX, _T("[Price_max]"), m_Price_max);
	DFX_Double(pFX, _T("[Doh_by_price_min]"), m_Doh_by_price_min);
	DFX_Double(pFX, _T("[Don_avg]"), m_Don_avg);
	DFX_Double(pFX, _T("[Cupon_nakop]"), m_Cupon_nakop);
	DFX_Double(pFX, _T("[Cupon_persent]"), m_Cupon_persent);
	DFX_Text(pFX, _T("[Date_1]"), m_Date_1);
	DFX_Text(pFX, _T("[Date_2]"), m_Date_2);
	DFX_Text(pFX, _T("[Date_3]"), m_Date_3);
	DFX_Text(pFX, _T("[Date_4]"), m_Date_4);
*/
	//{{AFX_FIELD_MAP(CB50Set)
	pFX->SetFieldType(CDaoFieldExchange::outputColumn);
	DFX_Double(pFX, _T("[Состояние]"), m_column1);
	DFX_Text(pFX, _T("[Место_рег]"), m_column2);
	DFX_Double(pFX, _T("[Рег_ номер]"), m_column3);
	DFX_Text(pFX, _T("[Полное_наименование]"), m_column4);
	DFX_Text(pFX, _T("[Наименование]"), m_column5);
	DFX_Text(pFX, _T("[Краткое имя]"), m_column6);
	DFX_Double(pFX, _T("[Прав_Ф1]"), m_______1);
	DFX_Double(pFX, _T("[Правовая_Ф2]"), m___________2);
	DFX_Double(pFX, _T("[Почт_индекс]"), m_column7);
	DFX_Text(pFX, _T("[Почт_адрес]"), m_column8);
	DFX_Text(pFX, _T("[Телефон]"), m_column9);
	DFX_Text(pFX, _T("[Факс]"), m_column10);
	DFX_Text(pFX, _T("[Телекс]"), m_column11);
	DFX_Text(pFX, _T("[ФИО_Руковод]"), m_column12);
	DFX_Double(pFX, _T("[Уставный_фонд_т_руб]"), m_column13);
	DFX_Double(pFX, _T("[UF_WAL_TA]"), m_UF_WAL_TA);
	DFX_Double(pFX, _T("[UF_ED_WAL]"), m_UF_ED_WAL);
	DFX_Double(pFX, _T("[KOL_UHR__L]"), m_KOL_UHR__L);
	DFX_Double(pFX, _T("[KOL_UHR_FL]"), m_KOL_UHR_FL);
	DFX_Text(pFX, _T("[Осн_вид_деят]"), m_column14);
	DFX_Double(pFX, _T("[PRIZNAK_OB]"), m_PRIZNAK_OB);
	DFX_Double(pFX, _T("[Кол_раб_мест]"), m_column15);
	DFX_Double(pFX, _T("[Код_ОКПО]"), m_column16);
	DFX_Double(pFX, _T("[KOD_SOOGU]"), m_KOD_SOOGU);
	DFX_Text(pFX, _T("[KOD_SOATO]"), m_KOD_SOATO);
	DFX_Double(pFX, _T("[KOD_KOPF]"), m_KOD_KOPF);
	DFX_Double(pFX, _T("[KOD_KSF]"), m_KOD_KSF);
	DFX_Text(pFX, _T("[Банк_реквизиты]"), m_column17);
	DFX_Double(pFX, _T("[Налог_округ]"), m_column18);
	DFX_Text(pFX, _T("[ИНН]"), m_column19);
	DFX_Text(pFX, _T("[Доп_информация]"), m_column20);
	DFX_DateTime(pFX, _T("[Дата_рег]"), m_column21);
	DFX_DateTime(pFX, _T("[Дата_изм]"), m_column22);
	DFX_DateTime(pFX, _T("[Дата_ликвидации]"), m_column23);
	DFX_Double(pFX, _T("[TIMECHG]"), m_TIMECHG);
	DFX_DateTime(pFX, _T("[DATECHG]"), m_DATECHG);
	DFX_Double(pFX, _T("[U_FIRM]"), m_U_FIRM);
	//}}AFX_FIELD_MAP
}
#ifdef _DEBUG
void CB50Set::AssertValid()const{CDaoRecordset::AssertValid();}
/*
void CB50Set::Dump(CDumpContext&dc)const
{
//	CDaoRecordset::Dump(dc);
}
*/
#endif //_DEBUG
