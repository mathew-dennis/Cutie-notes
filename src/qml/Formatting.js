.pragma library

function formatDate(date) {
	if (!date)
		return "";
	return Qt.formatDateTime(date, "dd/MM/yy h:mm ap");
}
