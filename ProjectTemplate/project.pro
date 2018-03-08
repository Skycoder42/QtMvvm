TEMPLATE = subdirs

SUBDIRS += \\
@if '%{CreateWidgets}'
	%{WidgetsName} \\
@endif
@if '%{CreateQuick}'
	%{QuickName} \\
@endif
	%{CoreName}

@if '%{CreateWidgets}'
%{WidgetsName}.depends += %{CoreName}
@endif
@if '%{CreateQuick}'
%{QuickName}.depends += %{CoreName}
@endif
