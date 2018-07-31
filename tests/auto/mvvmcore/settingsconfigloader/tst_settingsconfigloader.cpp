#include <QtTest>
#include <QtMvvmCore/private/settingsconfigloader_p.h>
using namespace QtMvvm;
using namespace QtMvvm::SettingsElements;

class SettingsConfigLoaderTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testConfigLoader_data();
	void testConfigLoader();

	void testDefaultIcon();

private:
	Setup createEntryDocumentSetup();
	Setup createGroupDocumentSetup();
	Setup createSectionDocumentSetup();
	Setup createCategoryDocumentSetup();

	Setup createSelectorsSetup0();
	Setup createSelectorsSetup1();
	Setup createSelectorsSetup2();
	Setup createSelectorsSetup3();
	Setup createSelectorsSetup4();

	Setup createIncludesSetup();
};

void SettingsConfigLoaderTest::testConfigLoader_data()
{
	QTest::addColumn<QString>("path");
	QTest::addColumn<QString>("frontend");
	QTest::addColumn<QStringList>("selectors");
	QTest::addColumn<bool>("success");
	QTest::addColumn<Setup>("setup");

	QTest::newRow("entryDocument") << QStringLiteral(SRCDIR "/entryDocument.xml")
								   << QStringLiteral("dummy")
								   << QStringList{}
								   << true
								   << createEntryDocumentSetup();

	QTest::newRow("groupDocument") << QStringLiteral(SRCDIR "/groupDocument.xml")
								   << QStringLiteral("dummy")
								   << QStringList{}
								   << true
								   << createGroupDocumentSetup();

	QTest::newRow("sectionDocument") << QStringLiteral(SRCDIR "/sectionDocument.xml")
									 << QStringLiteral("dummy")
									 << QStringList{}
									 << true
									 << createSectionDocumentSetup();

	QTest::newRow("categoryDocument") << QStringLiteral(SRCDIR "/categoryDocument.xml")
									  << QStringLiteral("dummy")
									  << QStringList{}
									  << true
									  << createCategoryDocumentSetup();

	QTest::newRow("selectorDocument.frontend.widgets") << QStringLiteral(SRCDIR "/selectorDocument.xml")
													   << QStringLiteral("widgets")
													   << QStringList{}
													   << true
													   << createSelectorsSetup0();

	QTest::newRow("selectorDocument.frontend.quick") << QStringLiteral(SRCDIR "/selectorDocument.xml")
													 << QStringLiteral("quick")
													 << QStringList{}
													 << true
													 << createSelectorsSetup1();

	QTest::newRow("selectorDocument.selector.typeA") << QStringLiteral(SRCDIR "/selectorDocument.xml")
													 << QStringLiteral("dummy")
													 << QStringList{
															QStringLiteral("a1"),
															QStringLiteral("b2"),
															QStringLiteral("a3")
														}
													 << true
													 << createSelectorsSetup2();

	QTest::newRow("selectorDocument.selector.typeB") << QStringLiteral(SRCDIR "/selectorDocument.xml")
													 << QStringLiteral("dummy")
													 << QStringList{
															QStringLiteral("b1"),
															QStringLiteral("a2"),
															QStringLiteral("b3")
														}
													 << true
													 << createSelectorsSetup3();

	QTest::newRow("selectorDocument.combined") << QStringLiteral(SRCDIR "/selectorDocument.xml")
											   << QStringLiteral("console")
											   << QStringList{
													  QStringLiteral("a1"),
													  QStringLiteral("a2"),
													  QStringLiteral("a3")
												  }
											   << true
											   << createSelectorsSetup4();

	QTest::newRow("includeDocument") << QStringLiteral(SRCDIR "/includeDocument.xml")
									 << QStringLiteral("dummy")
									 << QStringList{}
									 << true
									 << createIncludesSetup();

	QTest::newRow("invalidDocument") << QStringLiteral(SRCDIR "/invalidDocument.xml")
									 << QStringLiteral("dummy")
									 << QStringList{}
									 << false
									 << Setup{};
}

void SettingsConfigLoaderTest::testConfigLoader()
{
	QFETCH(QString, path);
	QFETCH(QString, frontend);
	QFETCH(QStringList, selectors);
	QFETCH(bool, success);
	QFETCH(Setup, setup);

	try {
		QFileSelector selector;
		selector.setExtraSelectors(selectors);
		SettingsConfigLoader loader;
		loader.changeDefaultIcon(QStringLiteral("qrc:/baum"));
		if(success) {
			auto res = loader.loadSetup(path, frontend, &selector);

			// compare low-level to be able to catch where errors happend easier
			QCOMPARE(res.allowRestore, setup.allowRestore);
			QCOMPARE(res.allowSearch, setup.allowSearch);
			QCOMPARE(res.categories.size(), setup.categories.size());
			for(auto i = 0; i < setup.categories.size(); i++) {
				const auto &resCat = res.categories[i];
				const auto &category = setup.categories[i];
				QCOMPARE(resCat.title, category.title);
				QCOMPARE(resCat.icon, category.icon);
				QCOMPARE(resCat.tooltip, category.tooltip);
				QCOMPARE(resCat.sections.size(), category.sections.size());
				for(auto j = 0; j < category.sections.size(); j++) {
					const auto &resSec = resCat.sections[j];
					const auto &section = category.sections[j];
					QCOMPARE(resSec.title, section.title);
					QCOMPARE(resSec.icon, section.icon);
					QCOMPARE(resSec.tooltip, section.tooltip);
					QCOMPARE(resSec.groups.size(), section.groups.size());
					for(auto k = 0; k < section.groups.size(); k++) {
						const auto &resGrp = resSec.groups[k];
						const auto &group = section.groups[k];
						QCOMPARE(resGrp.title, group.title);
						QCOMPARE(resGrp.tooltip, group.tooltip);
						QCOMPARE(resGrp.entries.size(), group.entries.size());
						for(auto l = 0; l < group.entries.size(); l++) {
							const auto &resEnt = resGrp.entries[l];
							const auto &entry = group.entries[l];
							QCOMPARE(resEnt.key, entry.key);
							QCOMPARE(resEnt.type, entry.type);
							QCOMPARE(resEnt.title, entry.title);
							QCOMPARE(resEnt.tooltip, entry.tooltip);
							QCOMPARE(resEnt.defaultValue, entry.defaultValue);
							QCOMPARE(resEnt.searchKeys, entry.searchKeys);
							QCOMPARE(resEnt.properties, entry.properties);
						}
					}
				}
			}
		} else
			QVERIFY_EXCEPTION_THROWN(loader.loadSetup(path, frontend, &selector), SettingsConfigException);
	} catch(std::exception &e) {
		if(success)
			QFAIL(e.what());
	}
}

void SettingsConfigLoaderTest::testDefaultIcon()
{
	try {
		QFileSelector selector;
		SettingsConfigLoader loader;
		auto res = loader.loadSetup(QStringLiteral(SRCDIR "/entryDocument.xml"), QStringLiteral("dummy"), &selector);
		QCOMPARE(res.categories.first().icon, QStringLiteral("qrc:/de/skycoder42/qtmvvm/icons/settings.svg"));
	} catch(std::exception &e) {
		QFAIL(e.what());
	}
}

Setup SettingsConfigLoaderTest::createEntryDocumentSetup()
{
	return Setup {
		true, true, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("minimal"), "bool"
									},
									Entry {
										QStringLiteral("full"), "string",
										QStringLiteral("FULL"), QStringLiteral("full tip"),
										QStringLiteral("hello world")
									},
									Entry {
										QStringLiteral("search"), "int",
										{}, {}, {},
										{QStringLiteral("hello"), QStringLiteral("world")}
									}, Entry {
										QStringLiteral("elements"), "double",
										{}, {}, {}, {},
										{
											{QStringLiteral("simple"), 42},
											{QStringLiteral("literal"), QStringLiteral("baum42")},
											{QStringLiteral("list"), QVariantList {
												13,
												QVariantList{4.2},
												QVariantMap{{QStringLiteral("simple"), 42}}
											}},
											{QStringLiteral("map"), QVariantMap{
												{QStringLiteral("simple"), 42},
												{QStringLiteral("list"), QVariantList{4.2}}
											}}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	};
}

Setup SettingsConfigLoaderTest::createGroupDocumentSetup()
{
	return Setup {
		true, true, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {},
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("minimal"), "bool"
									}
								}
							},
							Group {
								QStringLiteral("FULL"), QStringLiteral("full"), {
									Entry {
										QStringLiteral("a"), "bool"
									},
									Entry {
										QStringLiteral("b"), "bool"
									}
								}
							}
						}
					}
				}
			}
		}
	};
}

Setup SettingsConfigLoaderTest::createSectionDocumentSetup()
{
	return Setup {
		false, false, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General")
					},
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("direct"), "double"
									}
								}
							}
						}
					},
					Section {
						QStringLiteral("master"), QStringLiteral("file:///path/to/vengance.png"),
						QStringLiteral("ruler"), {
							Group {},
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("indirect"), "double"
									}
								}
							}
						}
					}
				}
			}
		}
	};
}

Setup SettingsConfigLoaderTest::createCategoryDocumentSetup()
{
	return Setup {
		false, false, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum")
			},
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("direct"), "double"
									}
								}
							}
						}
					}
				}
			},
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {},
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("indirect"), "double"
									}
								}
							}
						}
					}
				}
			},
			Category {
				QStringLiteral("main"), QStringLiteral("http://example.com/favicon.png"),
				QStringLiteral("another tooltip"), {
					Section {
						QStringLiteral("General")
					},
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("subdirect"), "double"
									}
								}
							}
						}
					},
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {},
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("fardirect"), "double"
									}
								}
							}
						}
					}
				}
			}
		}
	};
}

Setup SettingsConfigLoaderTest::createSelectorsSetup0()
{
	return Setup {
		false, false, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum")
			},
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {},
							Group {}
						}
					}
				}
			},

			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {}
						}
					}
				}
			}
		}
	};
}

Setup SettingsConfigLoaderTest::createSelectorsSetup1()
{
	return Setup {
		false, false, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General")
					},
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("t1"), "bool"
									}
								}
							}
						}
					}
				}
			},

			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {}
						}
					}
				}
			}
		}
	};
}

Setup SettingsConfigLoaderTest::createSelectorsSetup2()
{
	return Setup {
		false, false, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {}
						}
					}
				}
			},

			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General")
					},
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("t2"), "bool"
									}
								}
							}
						}
					}
				}
			}
		}
	};
}

Setup SettingsConfigLoaderTest::createSelectorsSetup3()
{
	return Setup {
		false, false, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {}
						}
					}
				}
			},

			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum")
			},
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General")
					},
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {}
						}
					}
				}
			}
		}
	};
}

Setup SettingsConfigLoaderTest::createSelectorsSetup4()
{
	return Setup {
		false, false, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {},
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("t1"), "bool"
									}
								}
							}
						}
					}
				}
			},

			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General")
					},
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {},
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("t2"), "bool"
									}
								}
							}
						}
					}
				}
			},

			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum")
			}
		}
	};
}

Setup SettingsConfigLoaderTest::createIncludesSetup()
{
	return Setup{
		false, false, {
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("included"), "url"
									}
								}
							}
						}
					}
				}
			},
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum"), {}, {
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("included"), "url"
									}
								}
							}
						}
					},
					Section {
						QStringLiteral("General"), {}, {}, {
							Group {},
							Group {QStringLiteral("incGroup")},
							Group {
								{}, {}, {
									Entry {
										QStringLiteral("incEntry"), "bool"
									}
								}
							}
						}
					}
				}
			},
			Category {
				QStringLiteral("General Settings"), QStringLiteral("qrc:/baum")
			}
		}
	};
}

QTEST_MAIN(SettingsConfigLoaderTest)

#include "tst_settingsconfigloader.moc"
