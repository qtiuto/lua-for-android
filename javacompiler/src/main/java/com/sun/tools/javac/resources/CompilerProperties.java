package com.sun.tools.javac.resources;

import com.sun.tools.javac.util.JCDiagnostic.*;
import com.sun.tools.javac.util.JCDiagnostic.Error;

public class CompilerProperties {
	public static class Notes {
		public static Note VerboseResolveMulti(Object... args){
			return new Note("compiler","verbose.resolve.multi",args);
		}

		public static Note ProcMessager(Object... args){
			return new Note("compiler","proc.messager",args);
		}

		public static Note RemovalPluralAdditional(Object... args){
			return new Note("compiler","removal.plural.additional",args);
		}

		public static final Note RemovalPluralAdditional = new Note("compiler","removal.plural.additional");

		public static Note DeprecatedRecompile(Object... args){
			return new Note("compiler","deprecated.recompile",args);
		}

		public static final Note DeprecatedRecompile = new Note("compiler","deprecated.recompile");

		public static Note UncheckedPlural(Object... args){
			return new Note("compiler","unchecked.plural",args);
		}

		public static final Note UncheckedPlural = new Note("compiler","unchecked.plural");

		public static Note RemovalRecompile(Object... args){
			return new Note("compiler","removal.recompile",args);
		}

		public static final Note RemovalRecompile = new Note("compiler","removal.recompile");

		public static Note UncheckedPluralAdditional(Object... args){
			return new Note("compiler","unchecked.plural.additional",args);
		}

		public static final Note UncheckedPluralAdditional = new Note("compiler","unchecked.plural.additional");

		public static Note RemovalFilenameAdditional(Object... args){
			return new Note("compiler","removal.filename.additional",args);
		}

		public static Note MrefStat1(Object... args){
			return new Note("compiler","mref.stat.1",args);
		}

		public static Note DeprecatedFilenameAdditional(Object... args){
			return new Note("compiler","deprecated.filename.additional",args);
		}

		public static Note Note(Object... args){
			return new Note("compiler","note",args);
		}

		public static final Note Note = new Note("compiler","note");

		public static Note UncheckedFilenameAdditional(Object... args){
			return new Note("compiler","unchecked.filename.additional",args);
		}

		public static Note UncheckedFilename(Object... args){
			return new Note("compiler","unchecked.filename",args);
		}

		public static Note UncheckedRecompile(Object... args){
			return new Note("compiler","unchecked.recompile",args);
		}

		public static final Note UncheckedRecompile = new Note("compiler","unchecked.recompile");

		public static Note DeferredMethodInst(Object... args){
			return new Note("compiler","deferred.method.inst",args);
		}

		public static Note VerboseResolveMulti1(Object... args){
			return new Note("compiler","verbose.resolve.multi.1",args);
		}

		public static Note DeprecatedPluralAdditional(Object... args){
			return new Note("compiler","deprecated.plural.additional",args);
		}

		public static final Note DeprecatedPluralAdditional = new Note("compiler","deprecated.plural.additional");

		public static Note MultipleElements(Object... args){
			return new Note("compiler","multiple.elements",args);
		}

		public static Note RemovalFilename(Object... args){
			return new Note("compiler","removal.filename",args);
		}

		public static Note RemovalPlural(Object... args){
			return new Note("compiler","removal.plural",args);
		}

		public static final Note RemovalPlural = new Note("compiler","removal.plural");

		public static Note DeprecatedPlural(Object... args){
			return new Note("compiler","deprecated.plural",args);
		}

		public static final Note DeprecatedPlural = new Note("compiler","deprecated.plural");

		public static Note CompressedDiags(Object... args){
			return new Note("compiler","compressed.diags",args);
		}

		public static final Note CompressedDiags = new Note("compiler","compressed.diags");

		public static Note MrefStat(Object... args){
			return new Note("compiler","mref.stat",args);
		}

		public static Note DeprecatedFilename(Object... args){
			return new Note("compiler","deprecated.filename",args);
		}

		public static Note LambdaStat(Object... args){
			return new Note("compiler","lambda.stat",args);
		}

	}
	public static class Warnings {
		public static Warning EmptyIf(Object... args){
			return new Warning("compiler","empty.if",args);
		}

		public static final Warning EmptyIf = new Warning("compiler","empty.if");

		public static Warning UnreachableCatch1(Object... args){
			return new Warning("compiler","unreachable.catch.1",args);
		}

		public static Warning RequiresAutomatic(Object... args){
			return new Warning("compiler","requires.automatic",args);
		}

		public static final Warning RequiresAutomatic = new Warning("compiler","requires.automatic");

		public static Warning ProcPackageDoesNotExist(Object... args){
			return new Warning("compiler","proc.package.does.not.exist",args);
		}

		public static Warning PackageEmptyOrNotFound(Object... args){
			return new Warning("compiler","package.empty.or.not.found",args);
		}

		public static Warning ModuleNotFound(Object... args){
			return new Warning("compiler","module.not.found",args);
		}

		public static Warning PositionOverflow(Object... args){
			return new Warning("compiler","position.overflow",args);
		}

		public static Warning OverrideBridge(Object... args){
			return new Warning("compiler","override.bridge",args);
		}

		public static Warning UncheckedAssignToVar(Object... args){
			return new Warning("compiler","unchecked.assign.to.var",args);
		}

		public static Warning UncheckedVarargsNonReifiableType(Object... args){
			return new Warning("compiler","unchecked.varargs.non.reifiable.type",args);
		}

		public static Warning DirPathElementNotDirectory(Object... args){
			return new Warning("compiler","dir.path.element.not.directory",args);
		}

		public static Warning LocnUnknownFileOnModulePath(Object... args){
			return new Warning("compiler","locn.unknown.file.on.module.path",args);
		}

		public static Warning LintOption(Object... args){
			return new Warning("compiler","lintOption",args);
		}

		public static Warning UncheckedMethInvocationApplied(Object... args){
			return new Warning("compiler","unchecked.meth.invocation.applied",args);
		}

		public static Warning HasBeenDeprecatedModule(Object... args){
			return new Warning("compiler","has.been.deprecated.module",args);
		}

		public static Warning UnexpectedArchiveFile(Object... args){
			return new Warning("compiler","unexpected.archive.file",args);
		}

		public static Warning FinallyCannotComplete(Object... args){
			return new Warning("compiler","finally.cannot.complete",args);
		}

		public static final Warning FinallyCannotComplete = new Warning("compiler","finally.cannot.complete");

		public static Warning ProcUseImplicit(Object... args){
			return new Warning("compiler","proc.use.implicit",args);
		}

		public static final Warning ProcUseImplicit = new Warning("compiler","proc.use.implicit");

		public static Warning RequiresTransitiveAutomatic(Object... args){
			return new Warning("compiler","requires.transitive.automatic",args);
		}

		public static final Warning RequiresTransitiveAutomatic = new Warning("compiler","requires.transitive.automatic");

		public static Warning VarargsRedundantTrustmeAnno(Object... args){
			return new Warning("compiler","varargs.redundant.trustme.anno",args);
		}

		public static Warning TryExplicitCloseCall(Object... args){
			return new Warning("compiler","try.explicit.close.call",args);
		}

		public static final Warning TryExplicitCloseCall = new Warning("compiler","try.explicit.close.call");

		public static Warning OverrideUncheckedThrown(Object... args){
			return new Warning("compiler","override.unchecked.thrown",args);
		}

		public static Warning DirPathElementNotFound(Object... args){
			return new Warning("compiler","dir.path.element.not.found",args);
		}

		public static Warning IllegalCharForEncoding(Object... args){
			return new Warning("compiler","illegal.char.for.encoding",args);
		}

		public static Warning BadNameForOption(Object... args){
			return new Warning("compiler","bad.name.for.option",args);
		}

		public static Warning SourceNoBootclasspath(Object... args){
			return new Warning("compiler","source.no.bootclasspath",args);
		}

		public static Warning InexactNonVarargsCall(Object... args){
			return new Warning("compiler","inexact.non-varargs.call",args);
		}

		public static Warning RawClassUse(Object... args){
			return new Warning("compiler","raw.class.use",args);
		}

		public static Warning UncheckedAssign(Object... args){
			return new Warning("compiler","unchecked.assign",args);
		}

		public static Warning ProcUnclosedTypeFiles(Object... args){
			return new Warning("compiler","proc.unclosed.type.files",args);
		}

		public static Warning FutureAttr(Object... args){
			return new Warning("compiler","future.attr",args);
		}

		public static Warning LongSVUID(Object... args){
			return new Warning("compiler","long.SVUID",args);
		}

		public static Warning PoorChoiceForModuleName(Object... args){
			return new Warning("compiler","poor.choice.for.module.name",args);
		}

		public static Warning LeaksNotAccessibleNotRequiredTransitive(Object... args){
			return new Warning("compiler","leaks.not.accessible.not.required.transitive",args);
		}

		public static Warning InvalidArchiveFile(Object... args){
			return new Warning("compiler","invalid.archive.file",args);
		}

		public static Warning ProcSuspiciousClassName(Object... args){
			return new Warning("compiler","proc.suspicious.class.name",args);
		}

		public static Warning ProcFileReopening(Object... args){
			return new Warning("compiler","proc.file.reopening",args);
		}

		public static Warning SelfRef(Object... args){
			return new Warning("compiler","self.ref",args);
		}

		public static Warning Warning(Object... args){
			return new Warning("compiler","warning",args);
		}

		public static final Warning Warning = new Warning("compiler","warning");

		public static Warning OutdirIsInExplodedModule(Object... args){
			return new Warning("compiler","outdir.is.in.exploded.module",args);
		}

		public static Warning TryResourceNotReferenced(Object... args){
			return new Warning("compiler","try.resource.not.referenced",args);
		}

		public static Warning PotentiallyAmbiguousOverload(Object... args){
			return new Warning("compiler","potentially.ambiguous.overload",args);
		}

		public static Warning OptionObsoleteTarget(Object... args){
			return new Warning("compiler","option.obsolete.target",args);
		}

		public static Warning UncheckedCallMbrOfRawType(Object... args){
			return new Warning("compiler","unchecked.call.mbr.of.raw.type",args);
		}

		public static Warning PossibleFallThroughIntoCase(Object... args){
			return new Warning("compiler","possible.fall-through.into.case",args);
		}

		public static final Warning PossibleFallThroughIntoCase = new Warning("compiler","possible.fall-through.into.case");

		public static Warning ProcUnmatchedProcessorOptions(Object... args){
			return new Warning("compiler","proc.unmatched.processor.options",args);
		}

		public static Warning LeaksNotAccessibleUnexported(Object... args){
			return new Warning("compiler","leaks.not.accessible.unexported",args);
		}

		public static Warning IncubatingModules(Object... args){
			return new Warning("compiler","incubating.modules",args);
		}

		public static Warning AuxiliaryClassAccessedFromOutsideOfItsSourceFile(Object... args){
			return new Warning("compiler","auxiliary.class.accessed.from.outside.of.its.source.file",args);
		}

		public static Warning HasBeenDeprecated(Object... args){
			return new Warning("compiler","has.been.deprecated",args);
		}

		public static Warning LeaksNotAccessibleUnexportedQualified(Object... args){
			return new Warning("compiler","leaks.not.accessible.unexported.qualified",args);
		}

		public static Warning AnnotationMethodNotFoundReason(Object... args){
			return new Warning("compiler","annotation.method.not.found.reason",args);
		}

		public static Warning UnknownEnumConstant(Object... args){
			return new Warning("compiler","unknown.enum.constant",args);
		}

		public static Warning MissingDeprecatedAnnotation(Object... args){
			return new Warning("compiler","missing.deprecated.annotation",args);
		}

		public static final Warning MissingDeprecatedAnnotation = new Warning("compiler","missing.deprecated.annotation");

		public static Warning FileFromFuture(Object... args){
			return new Warning("compiler","file.from.future",args);
		}

		public static Warning SunProprietary(Object... args){
			return new Warning("compiler","sun.proprietary",args);
		}

		public static Warning UncheckedCastToType(Object... args){
			return new Warning("compiler","unchecked.cast.to.type",args);
		}

		public static Warning OverrideEqualsButNotHashcode(Object... args){
			return new Warning("compiler","override.equals.but.not.hashcode",args);
		}

		public static Warning ProcIllegalFileName(Object... args){
			return new Warning("compiler","proc.illegal.file.name",args);
		}

		public static Warning DeprecatedAnnotationHasNoEffect(Object... args){
			return new Warning("compiler","deprecated.annotation.has.no.effect",args);
		}

		public static Warning ConstantSVUID(Object... args){
			return new Warning("compiler","constant.SVUID",args);
		}

		public static Warning PathElementNotFound(Object... args){
			return new Warning("compiler","path.element.not.found",args);
		}

		public static Warning OverrideVarargsMissing(Object... args){
			return new Warning("compiler","override.varargs.missing",args);
		}

		public static Warning ProcProcessorIncompatibleSourceVersion(Object... args){
			return new Warning("compiler","proc.processor.incompatible.source.version",args);
		}

		public static Warning ProcUseProcOrImplicit(Object... args){
			return new Warning("compiler","proc.use.proc.or.implicit",args);
		}

		public static final Warning ProcUseProcOrImplicit = new Warning("compiler","proc.use.proc.or.implicit");

		public static Warning HasBeenDeprecatedForRemoval(Object... args){
			return new Warning("compiler","has.been.deprecated.for.removal",args);
		}

		public static Warning PotentialLambdaFound(Object... args){
			return new Warning("compiler","potential.lambda.found",args);
		}

		public static final Warning PotentialLambdaFound = new Warning("compiler","potential.lambda.found");

		public static Warning ProcFileCreateLastRound(Object... args){
			return new Warning("compiler","proc.file.create.last.round",args);
		}

		public static Warning StaticNotQualifiedByType(Object... args){
			return new Warning("compiler","static.not.qualified.by.type",args);
		}

		public static Warning VarargsUnsafeUseVarargsParam(Object... args){
			return new Warning("compiler","varargs.unsafe.use.varargs.param",args);
		}

		public static Warning ProcTypeRecreate(Object... args){
			return new Warning("compiler","proc.type.recreate",args);
		}

		public static Warning AccessToMemberFromSerializableLambda(Object... args){
			return new Warning("compiler","access.to.member.from.serializable.lambda",args);
		}

		public static Warning DivZero(Object... args){
			return new Warning("compiler","div.zero",args);
		}

		public static final Warning DivZero = new Warning("compiler","div.zero");

		public static Warning OptionObsoleteSuppression(Object... args){
			return new Warning("compiler","option.obsolete.suppression",args);
		}

		public static final Warning OptionObsoleteSuppression = new Warning("compiler","option.obsolete.suppression");

		public static Warning RedundantCast(Object... args){
			return new Warning("compiler","redundant.cast",args);
		}

		public static Warning BigMajorVersion(Object... args){
			return new Warning("compiler","big.major.version",args);
		}

		public static Warning AccessToMemberFromSerializableElement(Object... args){
			return new Warning("compiler","access.to.member.from.serializable.element",args);
		}

		public static Warning ProcTypeAlreadyExists(Object... args){
			return new Warning("compiler","proc.type.already.exists",args);
		}

		public static Warning ProcMalformedSupportedString(Object... args){
			return new Warning("compiler","proc.malformed.supported.string",args);
		}

		public static Warning OverrideVarargsExtra(Object... args){
			return new Warning("compiler","override.varargs.extra",args);
		}

		public static Warning LeaksNotAccessible(Object... args){
			return new Warning("compiler","leaks.not.accessible",args);
		}

		public static Warning AnnotationMethodNotFound(Object... args){
			return new Warning("compiler","annotation.method.not.found",args);
		}

		public static Warning OverrideUncheckedRet(Object... args){
			return new Warning("compiler","override.unchecked.ret",args);
		}

		public static Warning UnknownEnumConstantReason(Object... args){
			return new Warning("compiler","unknown.enum.constant.reason",args);
		}

		public static Warning OptionObsoleteSource(Object... args){
			return new Warning("compiler","option.obsolete.source",args);
		}

		public static Warning MissingSVUID(Object... args){
			return new Warning("compiler","missing.SVUID",args);
		}

		public static Warning DiamondRedundantArgs(Object... args){
			return new Warning("compiler","diamond.redundant.args",args);
		}

		public static final Warning DiamondRedundantArgs = new Warning("compiler","diamond.redundant.args");

		public static Warning UnderscoreAsIdentifier(Object... args){
			return new Warning("compiler","underscore.as.identifier",args);
		}

		public static final Warning UnderscoreAsIdentifier = new Warning("compiler","underscore.as.identifier");

		public static Warning ProbFoundReq(Object... args){
			return new Warning("compiler","prob.found.req",args);
		}

		public static Warning PkgInfoAlreadySeen(Object... args){
			return new Warning("compiler","pkg-info.already.seen",args);
		}

		public static Warning ProcMessager(Object... args){
			return new Warning("compiler","proc.messager",args);
		}

		public static Warning TryResourceThrowsInterruptedExc(Object... args){
			return new Warning("compiler","try.resource.throws.interrupted.exc",args);
		}

		public static Warning AddopensIgnored(Object... args){
			return new Warning("compiler","addopens.ignored",args);
		}

		public static final Warning AddopensIgnored = new Warning("compiler","addopens.ignored");

		public static Warning ImproperSVUID(Object... args){
			return new Warning("compiler","improper.SVUID",args);
		}

		public static Warning MethodRedundantTypeargs(Object... args){
			return new Warning("compiler","method.redundant.typeargs",args);
		}

		public static final Warning MethodRedundantTypeargs = new Warning("compiler","method.redundant.typeargs");

		public static Warning UnreachableCatch(Object... args){
			return new Warning("compiler","unreachable.catch",args);
		}

		public static Warning LocalRedundantType(Object... args){
			return new Warning("compiler","local.redundant.type",args);
		}

		public static final Warning LocalRedundantType = new Warning("compiler","local.redundant.type");

		public static Warning InvalidPath(Object... args){
			return new Warning("compiler","invalid.path",args);
		}

		public static Warning UncheckedGenericArrayCreation(Object... args){
			return new Warning("compiler","unchecked.generic.array.creation",args);
		}

		public static Warning ProcProcOnlyRequestedNoProcs(Object... args){
			return new Warning("compiler","proc.proc-only.requested.no.procs",args);
		}

		public static final Warning ProcProcOnlyRequestedNoProcs = new Warning("compiler","proc.proc-only.requested.no.procs");

		public static Warning HasBeenDeprecatedForRemovalModule(Object... args){
			return new Warning("compiler","has.been.deprecated.for.removal.module",args);
		}

		public static Warning ForwardRef(Object... args){
			return new Warning("compiler","forward.ref",args);
		}

		public static Warning ProcAnnotationsWithoutProcessors(Object... args){
			return new Warning("compiler","proc.annotations.without.processors",args);
		}

		public static Warning ServiceProvidedButNotExportedOrUsed(Object... args){
			return new Warning("compiler","service.provided.but.not.exported.or.used",args);
		}

		public static final Warning ServiceProvidedButNotExportedOrUsed = new Warning("compiler","service.provided.but.not.exported.or.used");

		public static Warning ModuleForOptionNotFound(Object... args){
			return new Warning("compiler","module.for.option.not.found",args);
		}

	}
	public static class Errors {
		public static Error MultiModuleOutdirCannotBeExplodedModule(Object... args){
			return new Error("compiler","multi-module.outdir.cannot.be.exploded.module",args);
		}

		public static Error FileSbOnSourceOrPatchPathForModule(Object... args){
			return new Error("compiler","file.sb.on.source.or.patch.path.for.module",args);
		}

		public static final Error FileSbOnSourceOrPatchPathForModule = new Error("compiler","file.sb.on.source.or.patch.path.for.module");

		public static Error EnumAsIdentifier(Object... args){
			return new Error("compiler","enum.as.identifier",args);
		}

		public static final Error EnumAsIdentifier = new Error("compiler","enum.as.identifier");

		public static Error CantInferLocalVarType(Object... args){
			return new Error("compiler","cant.infer.local.var.type",args);
		}

		public static Error MissingRetStmt(Object... args){
			return new Error("compiler","missing.ret.stmt",args);
		}

		public static final Error MissingRetStmt = new Error("compiler","missing.ret.stmt");

		public static Error IntfAnnotationMemberClash(Object... args){
			return new Error("compiler","intf.annotation.member.clash",args);
		}

		public static Error NativeMethCantHaveBody(Object... args){
			return new Error("compiler","native.meth.cant.have.body",args);
		}

		public static final Error NativeMethCantHaveBody = new Error("compiler","native.meth.cant.have.body");

		public static Error AlreadyDefinedStaticSingleImport(Object... args){
			return new Error("compiler","already.defined.static.single.import",args);
		}

		public static Error AlreadyDefinedThisUnit(Object... args){
			return new Error("compiler","already.defined.this.unit",args);
		}

		public static Error TryWithResourcesNotSupportedInSource(Object... args){
			return new Error("compiler","try.with.resources.not.supported.in.source",args);
		}

		public static Error VarargsMustBeLast(Object... args){
			return new Error("compiler","varargs.must.be.last",args);
		}

		public static final Error VarargsMustBeLast = new Error("compiler","varargs.must.be.last");

		public static Error InvalidRepeatableAnnotationRepeatedAndContainerPresent(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.repeated.and.container.present",args);
		}

		public static Error TypesIncompatibleUnrelatedDefaults(Object... args){
			return new Error("compiler","types.incompatible.unrelated.defaults",args);
		}

		public static Error CantApplyDiamond1(Object... args){
			return new Error("compiler","cant.apply.diamond.1",args);
		}

		public static Error IntNumberTooLarge(Object... args){
			return new Error("compiler","int.number.too.large",args);
		}

		public static Error FilePatchedAndMsp(Object... args){
			return new Error("compiler","file.patched.and.msp",args);
		}

		public static Error ClashWithPkgOfSameName(Object... args){
			return new Error("compiler","clash.with.pkg.of.same.name",args);
		}

		public static Error LocnCantReadFile(Object... args){
			return new Error("compiler","locn.cant.read.file",args);
		}

		public static Error UnsupportedUnderscoreLit(Object... args){
			return new Error("compiler","unsupported.underscore.lit",args);
		}

		public static Error NoZipfsForArchive(Object... args){
			return new Error("compiler","no.zipfs.for.archive",args);
		}

		public static Error UnreachableStmt(Object... args){
			return new Error("compiler","unreachable.stmt",args);
		}

		public static final Error UnreachableStmt = new Error("compiler","unreachable.stmt");

		public static Error OverrideMeth(Object... args){
			return new Error("compiler","override.meth",args);
		}

		public static Error DuplicateCaseLabel(Object... args){
			return new Error("compiler","duplicate.case.label",args);
		}

		public static final Error DuplicateCaseLabel = new Error("compiler","duplicate.case.label");

		public static Error NotInProfile(Object... args){
			return new Error("compiler","not.in.profile",args);
		}

		public static Error AnonymousDiamondMethodDoesNotOverrideSuperclass(Object... args){
			return new Error("compiler","anonymous.diamond.method.does.not.override.superclass",args);
		}

		public static Error DoesntExist(Object... args){
			return new Error("compiler","doesnt.exist",args);
		}

		public static Error EnumsMustBeStatic(Object... args){
			return new Error("compiler","enums.must.be.static",args);
		}

		public static final Error EnumsMustBeStatic = new Error("compiler","enums.must.be.static");

		public static Error CantResolveLocationArgsParams(Object... args){
			return new Error("compiler","cant.resolve.location.args.params",args);
		}

		public static Error OverrideWeakerAccess(Object... args){
			return new Error("compiler","override.weaker.access",args);
		}

		public static Error IllegalChar(Object... args){
			return new Error("compiler","illegal.char",args);
		}

		public static Error DcBadEntity(Object... args){
			return new Error("compiler","dc.bad.entity",args);
		}

		public static final Error DcBadEntity = new Error("compiler","dc.bad.entity");

		public static Error ProcNoService(Object... args){
			return new Error("compiler","proc.no.service",args);
		}

		public static final Error ProcNoService = new Error("compiler","proc.no.service");

		public static Error UnreportedExceptionDefaultConstructor(Object... args){
			return new Error("compiler","unreported.exception.default.constructor",args);
		}

		public static Error DefaultMethodsNotSupportedInSource(Object... args){
			return new Error("compiler","default.methods.not.supported.in.source",args);
		}

		public static Error UnreportedExceptionImplicitClose(Object... args){
			return new Error("compiler","unreported.exception.implicit.close",args);
		}

		public static Error NameReservedForInternalUse(Object... args){
			return new Error("compiler","name.reserved.for.internal.use",args);
		}

		public static Error ProcServiceProblem(Object... args){
			return new Error("compiler","proc.service.problem",args);
		}

		public static final Error ProcServiceProblem = new Error("compiler","proc.service.problem");

		public static Error PrivateIntfMethodsNotSupportedInSource(Object... args){
			return new Error("compiler","private.intf.methods.not.supported.in.source",args);
		}

		public static Error ModuleNameMismatch(Object... args){
			return new Error("compiler","module.name.mismatch",args);
		}

		public static Error LimitParameters(Object... args){
			return new Error("compiler","limit.parameters",args);
		}

		public static final Error LimitParameters = new Error("compiler","limit.parameters");

		public static Error IllegalLineEndInCharLit(Object... args){
			return new Error("compiler","illegal.line.end.in.char.lit",args);
		}

		public static final Error IllegalLineEndInCharLit = new Error("compiler","illegal.line.end.in.char.lit");

		public static Error LocalVarAccessedFromIclsNeedsFinal(Object... args){
			return new Error("compiler","local.var.accessed.from.icls.needs.final",args);
		}

		public static Error DuplicateRequires(Object... args){
			return new Error("compiler","duplicate.requires",args);
		}

		public static Error LimitString(Object... args){
			return new Error("compiler","limit.string",args);
		}

		public static final Error LimitString = new Error("compiler","limit.string");

		public static Error AddReadsWithRelease(Object... args){
			return new Error("compiler","add.reads.with.release",args);
		}

		public static Error CyclicInheritance(Object... args){
			return new Error("compiler","cyclic.inheritance",args);
		}

		public static Error IntersectionTypesInCastNotSupportedInSource(Object... args){
			return new Error("compiler","intersection.types.in.cast.not.supported.in.source",args);
		}

		public static Error AlreadyDefinedSingleImport(Object... args){
			return new Error("compiler","already.defined.single.import",args);
		}

		public static Error ProcMessager(Object... args){
			return new Error("compiler","proc.messager",args);
		}

		public static Error CyclicRequires(Object... args){
			return new Error("compiler","cyclic.requires",args);
		}

		public static Error DuplicateDefaultLabel(Object... args){
			return new Error("compiler","duplicate.default.label",args);
		}

		public static final Error DuplicateDefaultLabel = new Error("compiler","duplicate.default.label");

		public static Error NoEnclInstanceOfTypeInScope(Object... args){
			return new Error("compiler","no.encl.instance.of.type.in.scope",args);
		}

		public static Error TryResourceMayNotBeAssigned(Object... args){
			return new Error("compiler","try.resource.may.not.be.assigned",args);
		}

		public static Error ServiceImplementationNotInRightModule(Object... args){
			return new Error("compiler","service.implementation.not.in.right.module",args);
		}

		public static final Error ServiceImplementationNotInRightModule = new Error("compiler","service.implementation.not.in.right.module");

		public static Error EmptyCharLit(Object... args){
			return new Error("compiler","empty.char.lit",args);
		}

		public static final Error EmptyCharLit = new Error("compiler","empty.char.lit");

		public static Error TooManyModules(Object... args){
			return new Error("compiler","too.many.modules",args);
		}

		public static final Error TooManyModules = new Error("compiler","too.many.modules");

		public static Error InvalidRepeatableAnnotationNoValue(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.no.value",args);
		}

		public static Error ModulesourcepathMustBeSpecifiedWithDashMOption(Object... args){
			return new Error("compiler","modulesourcepath.must.be.specified.with.dash.m.option",args);
		}

		public static final Error ModulesourcepathMustBeSpecifiedWithDashMOption = new Error("compiler","modulesourcepath.must.be.specified.with.dash.m.option");

		public static Error EnumNoFinalize(Object... args){
			return new Error("compiler","enum.no.finalize",args);
		}

		public static final Error EnumNoFinalize = new Error("compiler","enum.no.finalize");

		public static Error EnumNoSubclassing(Object... args){
			return new Error("compiler","enum.no.subclassing",args);
		}

		public static final Error EnumNoSubclassing = new Error("compiler","enum.no.subclassing");

		public static Error InvalidRepeatableAnnotationRetention(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.retention",args);
		}

		public static Error EnumCantBeInstantiated(Object... args){
			return new Error("compiler","enum.cant.be.instantiated",args);
		}

		public static final Error EnumCantBeInstantiated = new Error("compiler","enum.cant.be.instantiated");

		public static Error IntfNotAllowedHere(Object... args){
			return new Error("compiler","intf.not.allowed.here",args);
		}

		public static final Error IntfNotAllowedHere = new Error("compiler","intf.not.allowed.here");

		public static Error NoAnnotationMember(Object... args){
			return new Error("compiler","no.annotation.member",args);
		}

		public static Error BadFunctionalIntfAnno(Object... args){
			return new Error("compiler","bad.functional.intf.anno",args);
		}

		public static final Error BadFunctionalIntfAnno = new Error("compiler","bad.functional.intf.anno");

		public static Error IllegalEscChar(Object... args){
			return new Error("compiler","illegal.esc.char",args);
		}

		public static final Error IllegalEscChar = new Error("compiler","illegal.esc.char");

		public static Error NotStmt(Object... args){
			return new Error("compiler","not.stmt",args);
		}

		public static final Error NotStmt = new Error("compiler","not.stmt");

		public static Error AnnotationTypeNotApplicableToType(Object... args){
			return new Error("compiler","annotation.type.not.applicable.to.type",args);
		}

		public static Error CantResolveLocationArgs(Object... args){
			return new Error("compiler","cant.resolve.location.args",args);
		}

		public static Error AnnotationNotValidForType(Object... args){
			return new Error("compiler","annotation.not.valid.for.type",args);
		}

		public static Error DcMalformedHtml(Object... args){
			return new Error("compiler","dc.malformed.html",args);
		}

		public static final Error DcMalformedHtml = new Error("compiler","dc.malformed.html");

		public static Error InvalidHexNumber(Object... args){
			return new Error("compiler","invalid.hex.number",args);
		}

		public static final Error InvalidHexNumber = new Error("compiler","invalid.hex.number");

		public static Error NotDefAccessClassIntfCantAccessReason(Object... args){
			return new Error("compiler","not.def.access.class.intf.cant.access.reason",args);
		}

		public static Error ModuleDeclSbInModuleInfoJava(Object... args){
			return new Error("compiler","module.decl.sb.in.module-info.java",args);
		}

		public static final Error ModuleDeclSbInModuleInfoJava = new Error("compiler","module.decl.sb.in.module-info.java");

		public static Error NameClashSameErasureNoOverride1(Object... args){
			return new Error("compiler","name.clash.same.erasure.no.override.1",args);
		}

		public static Error StaticMethodsCannotBeAnnotatedWithOverride(Object... args){
			return new Error("compiler","static.methods.cannot.be.annotated.with.override",args);
		}

		public static final Error StaticMethodsCannotBeAnnotatedWithOverride = new Error("compiler","static.methods.cannot.be.annotated.with.override");

		public static Error StaticIntfMethodInvokeNotSupportedInSource(Object... args){
			return new Error("compiler","static.intf.method.invoke.not.supported.in.source",args);
		}

		public static Error DcRefUnexpectedInput(Object... args){
			return new Error("compiler","dc.ref.unexpected.input",args);
		}

		public static final Error DcRefUnexpectedInput = new Error("compiler","dc.ref.unexpected.input");

		public static Error EnclClassRequired(Object... args){
			return new Error("compiler","encl.class.required",args);
		}

		public static Error LimitPool(Object... args){
			return new Error("compiler","limit.pool",args);
		}

		public static final Error LimitPool = new Error("compiler","limit.pool");

		public static Error RepeatableAnnotationsNotSupportedInSource(Object... args){
			return new Error("compiler","repeatable.annotations.not.supported.in.source",args);
		}

		public static Error UnsupportedEncoding(Object... args){
			return new Error("compiler","unsupported.encoding",args);
		}

		public static Error ServiceImplementationIsInner(Object... args){
			return new Error("compiler","service.implementation.is.inner",args);
		}

		public static Error MethodDoesNotOverrideSuperclass(Object... args){
			return new Error("compiler","method.does.not.override.superclass",args);
		}

		public static final Error MethodDoesNotOverrideSuperclass = new Error("compiler","method.does.not.override.superclass");

		public static Error GenericThrowable(Object... args){
			return new Error("compiler","generic.throwable",args);
		}

		public static final Error GenericThrowable = new Error("compiler","generic.throwable");

		public static Error CantAccess(Object... args){
			return new Error("compiler","cant.access",args);
		}

		public static Error InvalidModuleSpecifier(Object... args){
			return new Error("compiler","invalid.module.specifier",args);
		}

		public static Error VarMightAlreadyBeAssigned(Object... args){
			return new Error("compiler","var.might.already.be.assigned",args);
		}

		public static Error EnumLabelMustBeUnqualifiedEnum(Object... args){
			return new Error("compiler","enum.label.must.be.unqualified.enum",args);
		}

		public static final Error EnumLabelMustBeUnqualifiedEnum = new Error("compiler","enum.label.must.be.unqualified.enum");

		public static Error NotLoopLabel(Object... args){
			return new Error("compiler","not.loop.label",args);
		}

		public static Error ProbFoundReq(Object... args){
			return new Error("compiler","prob.found.req",args);
		}

		public static Error TypeVarMoreThanOnce(Object... args){
			return new Error("compiler","type.var.more.than.once",args);
		}

		public static Error OverrideIncompatibleRet(Object... args){
			return new Error("compiler","override.incompatible.ret",args);
		}

		public static Error VarMightBeAssignedInLoop(Object... args){
			return new Error("compiler","var.might.be.assigned.in.loop",args);
		}

		public static Error VarNotAllowedHere(Object... args){
			return new Error("compiler","var.not.allowed.here",args);
		}

		public static final Error VarNotAllowedHere = new Error("compiler","var.not.allowed.here");

		public static Error LabelAlreadyInUse(Object... args){
			return new Error("compiler","label.already.in.use",args);
		}

		public static Error VarNotAllowed(Object... args){
			return new Error("compiler","var.not.allowed",args);
		}

		public static Error AnnotationValueMustBeClassLiteral(Object... args){
			return new Error("compiler","annotation.value.must.be.class.literal",args);
		}

		public static final Error AnnotationValueMustBeClassLiteral = new Error("compiler","annotation.value.must.be.class.literal");

		public static Error ConflictingExportsToModule(Object... args){
			return new Error("compiler","conflicting.exports.to.module",args);
		}

		public static Error IntfAnnotationMembersCantHaveTypeParams(Object... args){
			return new Error("compiler","intf.annotation.members.cant.have.type.params",args);
		}

		public static final Error IntfAnnotationMembersCantHaveTypeParams = new Error("compiler","intf.annotation.members.cant.have.type.params");

		public static Error TypeVarCantBeDeref(Object... args){
			return new Error("compiler","type.var.cant.be.deref",args);
		}

		public static final Error TypeVarCantBeDeref = new Error("compiler","type.var.cant.be.deref");

		public static Error CantRefNonEffectivelyFinalVar(Object... args){
			return new Error("compiler","cant.ref.non.effectively.final.var",args);
		}

		public static Error InvalidRepeatableAnnotationNotApplicable(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.not.applicable",args);
		}

		public static Error StringSwitchNotSupportedInSource(Object... args){
			return new Error("compiler","string.switch.not.supported.in.source",args);
		}

		public static Error IllegalCombinationOfModifiers(Object... args){
			return new Error("compiler","illegal.combination.of.modifiers",args);
		}

		public static Error TryWithoutCatchFinallyOrResourceDecls(Object... args){
			return new Error("compiler","try.without.catch.finally.or.resource.decls",args);
		}

		public static final Error TryWithoutCatchFinallyOrResourceDecls = new Error("compiler","try.without.catch.finally.or.resource.decls");

		public static Error IllegalGenericTypeForInstof(Object... args){
			return new Error("compiler","illegal.generic.type.for.instof",args);
		}

		public static final Error IllegalGenericTypeForInstof = new Error("compiler","illegal.generic.type.for.instof");

		public static Error DiamondNotSupportedInSource(Object... args){
			return new Error("compiler","diamond.not.supported.in.source",args);
		}

		public static Error Error(Object... args){
			return new Error("compiler","error",args);
		}

		public static final Error Error = new Error("compiler","error");

		public static Error IllegalStartOfExpr(Object... args){
			return new Error("compiler","illegal.start.of.expr",args);
		}

		public static final Error IllegalStartOfExpr = new Error("compiler","illegal.start.of.expr");

		public static Error NeitherConditionalSubtype(Object... args){
			return new Error("compiler","neither.conditional.subtype",args);
		}

		public static Error StackSimError(Object... args){
			return new Error("compiler","stack.sim.error",args);
		}

		public static Error NoAnnotationsOnDotClass(Object... args){
			return new Error("compiler","no.annotations.on.dot.class",args);
		}

		public static final Error NoAnnotationsOnDotClass = new Error("compiler","no.annotations.on.dot.class");

		public static Error AnnotationDeclNotAllowedHere(Object... args){
			return new Error("compiler","annotation.decl.not.allowed.here",args);
		}

		public static final Error AnnotationDeclNotAllowedHere = new Error("compiler","annotation.decl.not.allowed.here");

		public static Error PackageInOtherModule(Object... args){
			return new Error("compiler","package.in.other.module",args);
		}

		public static Error InvalidAnnotationMemberType(Object... args){
			return new Error("compiler","invalid.annotation.member.type",args);
		}

		public static final Error InvalidAnnotationMemberType = new Error("compiler","invalid.annotation.member.type");

		public static Error Expected(Object... args){
			return new Error("compiler","expected",args);
		}

		public static Error ProcCantAccess(Object... args){
			return new Error("compiler","proc.cant.access",args);
		}

		public static Error ServiceImplementationIsAbstract(Object... args){
			return new Error("compiler","service.implementation.is.abstract",args);
		}

		public static Error TypeDoesntTakeParams(Object... args){
			return new Error("compiler","type.doesnt.take.params",args);
		}

		public static Error AnnotationValueNotAllowableType(Object... args){
			return new Error("compiler","annotation.value.not.allowable.type",args);
		}

		public static final Error AnnotationValueNotAllowableType = new Error("compiler","annotation.value.not.allowable.type");

		public static Error CantInheritFromAnon(Object... args){
			return new Error("compiler","cant.inherit.from.anon",args);
		}

		public static final Error CantInheritFromAnon = new Error("compiler","cant.inherit.from.anon");

		public static Error CantAssignValToFinalVar(Object... args){
			return new Error("compiler","cant.assign.val.to.final.var",args);
		}

		public static Error MethodInvokedWithIncorrectNumberArguments(Object... args){
			return new Error("compiler","method.invoked.with.incorrect.number.arguments",args);
		}

		public static Error AssertAsIdentifier(Object... args){
			return new Error("compiler","assert.as.identifier",args);
		}

		public static final Error AssertAsIdentifier = new Error("compiler","assert.as.identifier");

		public static Error IllegalStaticIntfMethCall(Object... args){
			return new Error("compiler","illegal.static.intf.meth.call",args);
		}

		public static Error VarargsAndOldArraySyntax(Object... args){
			return new Error("compiler","varargs.and.old.array.syntax",args);
		}

		public static final Error VarargsAndOldArraySyntax = new Error("compiler","varargs.and.old.array.syntax");

		public static Error UnreportedExceptionNeedToCatchOrThrow(Object... args){
			return new Error("compiler","unreported.exception.need.to.catch.or.throw",args);
		}

		public static Error AttributeValueMustBeConstant(Object... args){
			return new Error("compiler","attribute.value.must.be.constant",args);
		}

		public static final Error AttributeValueMustBeConstant = new Error("compiler","attribute.value.must.be.constant");

		public static Error UnexpectedMref(Object... args){
			return new Error("compiler","unexpected.mref",args);
		}

		public static final Error UnexpectedMref = new Error("compiler","unexpected.mref");

		public static Error DcNoContent(Object... args){
			return new Error("compiler","dc.no.content",args);
		}

		public static final Error DcNoContent = new Error("compiler","dc.no.content");

		public static Error DuplicateAnnotationMissingContainer(Object... args){
			return new Error("compiler","duplicate.annotation.missing.container",args);
		}

		public static Error PackageEmptyOrNotFound(Object... args){
			return new Error("compiler","package.empty.or.not.found",args);
		}

		public static Error IncorrectReceiverType(Object... args){
			return new Error("compiler","incorrect.receiver.type",args);
		}

		public static Error RepeatedAnnotationTarget(Object... args){
			return new Error("compiler","repeated.annotation.target",args);
		}

		public static final Error RepeatedAnnotationTarget = new Error("compiler","repeated.annotation.target");

		public static Error IncomparableTypes(Object... args){
			return new Error("compiler","incomparable.types",args);
		}

		public static Error ServiceImplementationNoArgsConstructorNotPublic(Object... args){
			return new Error("compiler","service.implementation.no.args.constructor.not.public",args);
		}

		public static Error LimitDimensions(Object... args){
			return new Error("compiler","limit.dimensions",args);
		}

		public static final Error LimitDimensions = new Error("compiler","limit.dimensions");

		public static Error PkgAnnotationsSbInPackageInfoJava(Object... args){
			return new Error("compiler","pkg.annotations.sb.in.package-info.java",args);
		}

		public static final Error PkgAnnotationsSbInPackageInfoJava = new Error("compiler","pkg.annotations.sb.in.package-info.java");

		public static Error IllegalNonasciiDigit(Object... args){
			return new Error("compiler","illegal.nonascii.digit",args);
		}

		public static final Error IllegalNonasciiDigit = new Error("compiler","illegal.nonascii.digit");

		public static Error MissingMethBodyOrDeclAbstract(Object... args){
			return new Error("compiler","missing.meth.body.or.decl.abstract",args);
		}

		public static final Error MissingMethBodyOrDeclAbstract = new Error("compiler","missing.meth.body.or.decl.abstract");

		public static Error ConflictingOpensToModule(Object... args){
			return new Error("compiler","conflicting.opens.to.module",args);
		}

		public static Error VarNotAllowedArray(Object... args){
			return new Error("compiler","var.not.allowed.array",args);
		}

		public static final Error VarNotAllowedArray = new Error("compiler","var.not.allowed.array");

		public static Error OverrideMethDoesntThrow(Object... args){
			return new Error("compiler","override.meth.doesnt.throw",args);
		}

		public static Error MulticatchNotSupportedInSource(Object... args){
			return new Error("compiler","multicatch.not.supported.in.source",args);
		}

		public static Error MalformedFpLit(Object... args){
			return new Error("compiler","malformed.fp.lit",args);
		}

		public static final Error MalformedFpLit = new Error("compiler","malformed.fp.lit");

		public static Error InitializerNotAllowed(Object... args){
			return new Error("compiler","initializer.not.allowed",args);
		}

		public static final Error InitializerNotAllowed = new Error("compiler","initializer.not.allowed");

		public static Error PkgClashesWithClassOfSameName(Object... args){
			return new Error("compiler","pkg.clashes.with.class.of.same.name",args);
		}

		public static Error WarningsAndWerror(Object... args){
			return new Error("compiler","warnings.and.werror",args);
		}

		public static final Error WarningsAndWerror = new Error("compiler","warnings.and.werror");

		public static Error AddExportsWithRelease(Object... args){
			return new Error("compiler","add.exports.with.release",args);
		}

		public static Error ServiceDefinitionIsEnum(Object... args){
			return new Error("compiler","service.definition.is.enum",args);
		}

		public static Error DcIdentifierExpected(Object... args){
			return new Error("compiler","dc.identifier.expected",args);
		}

		public static final Error DcIdentifierExpected = new Error("compiler","dc.identifier.expected");

		public static Error ExceptNeverThrownInTry(Object... args){
			return new Error("compiler","except.never.thrown.in.try",args);
		}

		public static Error NoOutputDir(Object... args){
			return new Error("compiler","no.output.dir",args);
		}

		public static final Error NoOutputDir = new Error("compiler","no.output.dir");

		public static Error InvalidMethDeclRetTypeReq(Object... args){
			return new Error("compiler","invalid.meth.decl.ret.type.req",args);
		}

		public static final Error InvalidMethDeclRetTypeReq = new Error("compiler","invalid.meth.decl.ret.type.req");

		public static Error TryWithResourcesExprEffectivelyFinalVar(Object... args){
			return new Error("compiler","try.with.resources.expr.effectively.final.var",args);
		}

		public static Error AlreadyDefined(Object... args){
			return new Error("compiler","already.defined",args);
		}

		public static Error IncompatibleThrownTypesInMref(Object... args){
			return new Error("compiler","incompatible.thrown.types.in.mref",args);
		}

		public static Error LimitStack(Object... args){
			return new Error("compiler","limit.stack",args);
		}

		public static final Error LimitStack = new Error("compiler","limit.stack");

		public static Error CantDeref(Object... args){
			return new Error("compiler","cant.deref",args);
		}

		public static Error BadFunctionalIntfAnno1(Object... args){
			return new Error("compiler","bad.functional.intf.anno.1",args);
		}

		public static Error ReportAccess(Object... args){
			return new Error("compiler","report.access",args);
		}

		public static Error NewNotAllowedInAnnotation(Object... args){
			return new Error("compiler","new.not.allowed.in.annotation",args);
		}

		public static final Error NewNotAllowedInAnnotation = new Error("compiler","new.not.allowed.in.annotation");

		public static Error RepeatedProvidesForService(Object... args){
			return new Error("compiler","repeated.provides.for.service",args);
		}

		public static Error IllegalQualNotIcls(Object... args){
			return new Error("compiler","illegal.qual.not.icls",args);
		}

		public static Error OutputDirMustBeSpecifiedWithDashMOption(Object... args){
			return new Error("compiler","output.dir.must.be.specified.with.dash.m.option",args);
		}

		public static final Error OutputDirMustBeSpecifiedWithDashMOption = new Error("compiler","output.dir.must.be.specified.with.dash.m.option");

		public static Error IntfMethCantHaveBody(Object... args){
			return new Error("compiler","intf.meth.cant.have.body",args);
		}

		public static final Error IntfMethCantHaveBody = new Error("compiler","intf.meth.cant.have.body");

		public static Error ElseWithoutIf(Object... args){
			return new Error("compiler","else.without.if",args);
		}

		public static final Error ElseWithoutIf = new Error("compiler","else.without.if");

		public static Error AddmodsAllModulePathInvalid(Object... args){
			return new Error("compiler","addmods.all.module.path.invalid",args);
		}

		public static final Error AddmodsAllModulePathInvalid = new Error("compiler","addmods.all.module.path.invalid");

		public static Error StaticIntfMethodsNotSupportedInSource(Object... args){
			return new Error("compiler","static.intf.methods.not.supported.in.source",args);
		}

		public static Error RepeatedInterface(Object... args){
			return new Error("compiler","repeated.interface",args);
		}

		public static final Error RepeatedInterface = new Error("compiler","repeated.interface");

		public static Error ProcProcessorNotFound(Object... args){
			return new Error("compiler","proc.processor.not.found",args);
		}

		public static Error MethodReferencesNotSupportedInSource(Object... args){
			return new Error("compiler","method.references.not.supported.in.source",args);
		}

		public static Error IllegalArgumentForOption(Object... args){
			return new Error("compiler","illegal.argument.for.option",args);
		}

		public static Error DuplicateModuleOnPath(Object... args){
			return new Error("compiler","duplicate.module.on.path",args);
		}

		public static Error IllegalCharForEncoding(Object... args){
			return new Error("compiler","illegal.char.for.encoding",args);
		}

		public static Error CantExtendIntfAnnotation(Object... args){
			return new Error("compiler","cant.extend.intf.annotation",args);
		}

		public static final Error CantExtendIntfAnnotation = new Error("compiler","cant.extend.intf.annotation");

		public static Error RetOutsideMeth(Object... args){
			return new Error("compiler","ret.outside.meth",args);
		}

		public static final Error RetOutsideMeth = new Error("compiler","ret.outside.meth");

		public static Error IncorrectReceiverName(Object... args){
			return new Error("compiler","incorrect.receiver.name",args);
		}

		public static Error StaticImpOnlyClassesAndInterfaces(Object... args){
			return new Error("compiler","static.imp.only.classes.and.interfaces",args);
		}

		public static final Error StaticImpOnlyClassesAndInterfaces = new Error("compiler","static.imp.only.classes.and.interfaces");

		public static Error ExceptAlreadyCaught(Object... args){
			return new Error("compiler","except.already.caught",args);
		}

		public static Error ExpectedModuleOrOpen(Object... args){
			return new Error("compiler","expected.module.or.open",args);
		}

		public static final Error ExpectedModuleOrOpen = new Error("compiler","expected.module.or.open");

		public static Error InvalidBinaryNumber(Object... args){
			return new Error("compiler","invalid.binary.number",args);
		}

		public static final Error InvalidBinaryNumber = new Error("compiler","invalid.binary.number");

		public static Error CantTypeAnnotateScoping1(Object... args){
			return new Error("compiler","cant.type.annotate.scoping.1",args);
		}

		public static Error ContOutsideLoop(Object... args){
			return new Error("compiler","cont.outside.loop",args);
		}

		public static final Error ContOutsideLoop = new Error("compiler","cont.outside.loop");

		public static Error ImproperlyFormedTypeParamMissing(Object... args){
			return new Error("compiler","improperly.formed.type.param.missing",args);
		}

		public static final Error ImproperlyFormedTypeParamMissing = new Error("compiler","improperly.formed.type.param.missing");

		public static Error SourceCantOverwriteInputFile(Object... args){
			return new Error("compiler","source.cant.overwrite.input.file",args);
		}

		public static Error TypeFoundReq(Object... args){
			return new Error("compiler","type.found.req",args);
		}

		public static Error CallMustBeFirstStmtInCtor(Object... args){
			return new Error("compiler","call.must.be.first.stmt.in.ctor",args);
		}

		public static Error RefAmbiguous(Object... args){
			return new Error("compiler","ref.ambiguous",args);
		}

		public static Error ClassPublicShouldBeInFile(Object... args){
			return new Error("compiler","class.public.should.be.in.file",args);
		}

		public static Error DefaultOverridesObjectMember(Object... args){
			return new Error("compiler","default.overrides.object.member",args);
		}

		public static Error UnclosedComment(Object... args){
			return new Error("compiler","unclosed.comment",args);
		}

		public static final Error UnclosedComment = new Error("compiler","unclosed.comment");

		public static Error QualifiedNewOfStaticClass(Object... args){
			return new Error("compiler","qualified.new.of.static.class",args);
		}

		public static final Error QualifiedNewOfStaticClass = new Error("compiler","qualified.new.of.static.class");

		public static Error FinallyWithoutTry(Object... args){
			return new Error("compiler","finally.without.try",args);
		}

		public static final Error FinallyWithoutTry = new Error("compiler","finally.without.try");

		public static Error TypeVarMoreThanOnceInResult(Object... args){
			return new Error("compiler","type.var.more.than.once.in.result",args);
		}

		public static Error OperatorCantBeApplied(Object... args){
			return new Error("compiler","operator.cant.be.applied",args);
		}

		public static Error RecursiveCtorInvocation(Object... args){
			return new Error("compiler","recursive.ctor.invocation",args);
		}

		public static final Error RecursiveCtorInvocation = new Error("compiler","recursive.ctor.invocation");

		public static Error VoidNotAllowedHere(Object... args){
			return new Error("compiler","void.not.allowed.here",args);
		}

		public static final Error VoidNotAllowedHere = new Error("compiler","void.not.allowed.here");

		public static Error UnclosedStrLit(Object... args){
			return new Error("compiler","unclosed.str.lit",args);
		}

		public static final Error UnclosedStrLit = new Error("compiler","unclosed.str.lit");

		public static Error LocalEnum(Object... args){
			return new Error("compiler","local.enum",args);
		}

		public static final Error LocalEnum = new Error("compiler","local.enum");

		public static Error VarInTryWithResourcesNotSupportedInSource(Object... args){
			return new Error("compiler","var.in.try.with.resources.not.supported.in.source",args);
		}

		public static Error NameClashSameErasureNoOverride(Object... args){
			return new Error("compiler","name.clash.same.erasure.no.override",args);
		}

		public static Error NotInModuleOnModuleSourcePath(Object... args){
			return new Error("compiler","not.in.module.on.module.source.path",args);
		}

		public static final Error NotInModuleOnModuleSourcePath = new Error("compiler","not.in.module.on.module.source.path");

		public static Error IllegalStartOfType(Object... args){
			return new Error("compiler","illegal.start.of.type",args);
		}

		public static final Error IllegalStartOfType = new Error("compiler","illegal.start.of.type");

		public static Error LimitStringOverflow(Object... args){
			return new Error("compiler","limit.string.overflow",args);
		}

		public static Error IoException(Object... args){
			return new Error("compiler","io.exception",args);
		}

		public static Error LambdaNotSupportedInSource(Object... args){
			return new Error("compiler","lambda.not.supported.in.source",args);
		}

		public static Error ModuleNonZeroOpens(Object... args){
			return new Error("compiler","module.non.zero.opens",args);
		}

		public static Error ThisAsIdentifier(Object... args){
			return new Error("compiler","this.as.identifier",args);
		}

		public static final Error ThisAsIdentifier = new Error("compiler","this.as.identifier");

		public static Error NotDefAccessClassIntfCantAccess(Object... args){
			return new Error("compiler","not.def.access.class.intf.cant.access",args);
		}

		public static Error ConflictingOpens(Object... args){
			return new Error("compiler","conflicting.opens",args);
		}

		public static Error InvalidRepeatableAnnotationIncompatibleTarget(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.incompatible.target",args);
		}

		public static Error DuplicateAnnotationMemberValue(Object... args){
			return new Error("compiler","duplicate.annotation.member.value",args);
		}

		public static Error CantInheritFromFinal(Object... args){
			return new Error("compiler","cant.inherit.from.final",args);
		}

		public static Error NonStaticCantBeRef(Object... args){
			return new Error("compiler","non-static.cant.be.ref",args);
		}

		public static Error InvalidRepeatableAnnotationValueReturn(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.value.return",args);
		}

		public static Error IclsCantHaveStaticDecl(Object... args){
			return new Error("compiler","icls.cant.have.static.decl",args);
		}

		public static Error LambdaBodyNeitherValueNorVoidCompatible(Object... args){
			return new Error("compiler","lambda.body.neither.value.nor.void.compatible",args);
		}

		public static final Error LambdaBodyNeitherValueNorVoidCompatible = new Error("compiler","lambda.body.neither.value.nor.void.compatible");

		public static Error CyclicAnnotationElement(Object... args){
			return new Error("compiler","cyclic.annotation.element",args);
		}

		public static Error ServiceImplementationDoesntHaveANoArgsConstructor(Object... args){
			return new Error("compiler","service.implementation.doesnt.have.a.no.args.constructor",args);
		}

		public static Error InvalidRepeatableAnnotationInvalidValue(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.invalid.value",args);
		}

		public static Error CantSelectStaticClassFromParamType(Object... args){
			return new Error("compiler","cant.select.static.class.from.param.type",args);
		}

		public static final Error CantSelectStaticClassFromParamType = new Error("compiler","cant.select.static.class.from.param.type");

		public static Error CatchWithoutTry(Object... args){
			return new Error("compiler","catch.without.try",args);
		}

		public static final Error CatchWithoutTry = new Error("compiler","catch.without.try");

		public static Error IncorrectConstructorReceiverName(Object... args){
			return new Error("compiler","incorrect.constructor.receiver.name",args);
		}

		public static Error MulticatchTypesMustBeDisjoint(Object... args){
			return new Error("compiler","multicatch.types.must.be.disjoint",args);
		}

		public static Error AnnotationTypeNotApplicable(Object... args){
			return new Error("compiler","annotation.type.not.applicable",args);
		}

		public static final Error AnnotationTypeNotApplicable = new Error("compiler","annotation.type.not.applicable");

		public static Error DcRefSyntaxError(Object... args){
			return new Error("compiler","dc.ref.syntax.error",args);
		}

		public static final Error DcRefSyntaxError = new Error("compiler","dc.ref.syntax.error");

		public static Error PatchModuleWithRelease(Object... args){
			return new Error("compiler","patch.module.with.release",args);
		}

		public static Error ModuleNotFoundOnModuleSourcePath(Object... args){
			return new Error("compiler","module.not.found.on.module.source.path",args);
		}

		public static final Error ModuleNotFoundOnModuleSourcePath = new Error("compiler","module.not.found.on.module.source.path");

		public static Error IllegalForwardRef(Object... args){
			return new Error("compiler","illegal.forward.ref",args);
		}

		public static final Error IllegalForwardRef = new Error("compiler","illegal.forward.ref");

		public static Error NoOpensUnlessStrong(Object... args){
			return new Error("compiler","no.opens.unless.strong",args);
		}

		public static final Error NoOpensUnlessStrong = new Error("compiler","no.opens.unless.strong");

		public static Error ProcProcessorWrongType(Object... args){
			return new Error("compiler","proc.processor.wrong.type",args);
		}

		public static Error BreakOutsideSwitchLoop(Object... args){
			return new Error("compiler","break.outside.switch.loop",args);
		}

		public static final Error BreakOutsideSwitchLoop = new Error("compiler","break.outside.switch.loop");

		public static Error IntfAnnotationCantHaveTypeParams(Object... args){
			return new Error("compiler","intf.annotation.cant.have.type.params",args);
		}

		public static Error StringConstReq(Object... args){
			return new Error("compiler","string.const.req",args);
		}

		public static final Error StringConstReq = new Error("compiler","string.const.req");

		public static Error OverrideStatic(Object... args){
			return new Error("compiler","override.static",args);
		}

		public static Error DcBadInlineTag(Object... args){
			return new Error("compiler","dc.bad.inline.tag",args);
		}

		public static final Error DcBadInlineTag = new Error("compiler","dc.bad.inline.tag");

		public static Error NameClashSameErasureNoHide(Object... args){
			return new Error("compiler","name.clash.same.erasure.no.hide",args);
		}

		public static Error ExpectedModule(Object... args){
			return new Error("compiler","expected.module",args);
		}

		public static final Error ExpectedModule = new Error("compiler","expected.module");

		public static Error OperatorCantBeApplied1(Object... args){
			return new Error("compiler","operator.cant.be.applied.1",args);
		}

		public static Error CantApplyDiamond(Object... args){
			return new Error("compiler","cant.apply.diamond",args);
		}

		public static Error NotWithinBounds(Object... args){
			return new Error("compiler","not.within.bounds",args);
		}

		public static Error ThrowsNotAllowedInIntfAnnotation(Object... args){
			return new Error("compiler","throws.not.allowed.in.intf.annotation",args);
		}

		public static final Error ThrowsNotAllowedInIntfAnnotation = new Error("compiler","throws.not.allowed.in.intf.annotation");

		public static Error AnonClassImplIntfNoTypeargs(Object... args){
			return new Error("compiler","anon.class.impl.intf.no.typeargs",args);
		}

		public static final Error AnonClassImplIntfNoTypeargs = new Error("compiler","anon.class.impl.intf.no.typeargs");

		public static Error LimitPoolInClass(Object... args){
			return new Error("compiler","limit.pool.in.class",args);
		}

		public static Error AlreadyAnnotated(Object... args){
			return new Error("compiler","already.annotated",args);
		}

		public static Error PackageClashFromRequires(Object... args){
			return new Error("compiler","package.clash.from.requires",args);
		}

		public static Error DoesNotOverrideAbstract(Object... args){
			return new Error("compiler","does.not.override.abstract",args);
		}

		public static Error ProcNoExplicitAnnotationProcessingRequested(Object... args){
			return new Error("compiler","proc.no.explicit.annotation.processing.requested",args);
		}

		public static Error WrongNumberTypeArgs(Object... args){
			return new Error("compiler","wrong.number.type.args",args);
		}

		public static Error LocnCantReadDirectory(Object... args){
			return new Error("compiler","locn.cant.read.directory",args);
		}

		public static Error OptionRemovedSource(Object... args){
			return new Error("compiler","option.removed.source",args);
		}

		public static Error LocnBadModuleInfo(Object... args){
			return new Error("compiler","locn.bad.module-info",args);
		}

		public static Error IllegalDot(Object... args){
			return new Error("compiler","illegal.dot",args);
		}

		public static final Error IllegalDot = new Error("compiler","illegal.dot");

		public static Error ProcBadConfigFile(Object... args){
			return new Error("compiler","proc.bad.config.file",args);
		}

		public static Error Orphaned(Object... args){
			return new Error("compiler","orphaned",args);
		}

		public static Error AnnotationMissingDefaultValue1(Object... args){
			return new Error("compiler","annotation.missing.default.value.1",args);
		}

		public static Error ProcessorpathNoProcessormodulepath(Object... args){
			return new Error("compiler","processorpath.no.processormodulepath",args);
		}

		public static final Error ProcessorpathNoProcessormodulepath = new Error("compiler","processorpath.no.processormodulepath");

		public static Error UnclosedCharLit(Object... args){
			return new Error("compiler","unclosed.char.lit",args);
		}

		public static final Error UnclosedCharLit = new Error("compiler","unclosed.char.lit");

		public static Error LocnInvalidArgForXpatch(Object... args){
			return new Error("compiler","locn.invalid.arg.for.xpatch",args);
		}

		public static Error DefaultAllowedInIntfAnnotationMember(Object... args){
			return new Error("compiler","default.allowed.in.intf.annotation.member",args);
		}

		public static final Error DefaultAllowedInIntfAnnotationMember = new Error("compiler","default.allowed.in.intf.annotation.member");

		public static Error DcGtExpected(Object... args){
			return new Error("compiler","dc.gt.expected",args);
		}

		public static final Error DcGtExpected = new Error("compiler","dc.gt.expected");

		public static Error IllegalDefaultSuperCall(Object... args){
			return new Error("compiler","illegal.default.super.call",args);
		}

		public static Error ArrayAndReceiver(Object... args){
			return new Error("compiler","array.and.receiver",args);
		}

		public static final Error ArrayAndReceiver = new Error("compiler","array.and.receiver");

		public static Error TooManyPatchedModules(Object... args){
			return new Error("compiler","too.many.patched.modules",args);
		}

		public static Error ImproperlyFormedTypeInnerRawParam(Object... args){
			return new Error("compiler","improperly.formed.type.inner.raw.param",args);
		}

		public static final Error ImproperlyFormedTypeInnerRawParam = new Error("compiler","improperly.formed.type.inner.raw.param");

		public static Error UndefLabel(Object... args){
			return new Error("compiler","undef.label",args);
		}

		public static Error InvalidRepeatableAnnotation(Object... args){
			return new Error("compiler","invalid.repeatable.annotation",args);
		}

		public static Error ImportRequiresCanonical(Object... args){
			return new Error("compiler","import.requires.canonical",args);
		}

		public static Error AnnotationValueMustBeNameValue(Object... args){
			return new Error("compiler","annotation.value.must.be.name.value",args);
		}

		public static final Error AnnotationValueMustBeNameValue = new Error("compiler","annotation.value.must.be.name.value");

		public static Error InvalidModuleDirective(Object... args){
			return new Error("compiler","invalid.module.directive",args);
		}

		public static final Error InvalidModuleDirective = new Error("compiler","invalid.module.directive");

		public static Error ConflictingExports(Object... args){
			return new Error("compiler","conflicting.exports",args);
		}

		public static Error InitializerMustBeAbleToCompleteNormally(Object... args){
			return new Error("compiler","initializer.must.be.able.to.complete.normally",args);
		}

		public static final Error InitializerMustBeAbleToCompleteNormally = new Error("compiler","initializer.must.be.able.to.complete.normally");

		public static Error FpNumberTooLarge(Object... args){
			return new Error("compiler","fp.number.too.large",args);
		}

		public static final Error FpNumberTooLarge = new Error("compiler","fp.number.too.large");

		public static Error ForeachNotApplicableToType(Object... args){
			return new Error("compiler","foreach.not.applicable.to.type",args);
		}

		public static Error PluginNotFound(Object... args){
			return new Error("compiler","plugin.not.found",args);
		}

		public static Error CantApplySymbol(Object... args){
			return new Error("compiler","cant.apply.symbol",args);
		}

		public static Error AbstractCantBeInstantiated(Object... args){
			return new Error("compiler","abstract.cant.be.instantiated",args);
		}

		public static Error FpNumberTooSmall(Object... args){
			return new Error("compiler","fp.number.too.small",args);
		}

		public static final Error FpNumberTooSmall = new Error("compiler","fp.number.too.small");

		public static Error CannotCreateArrayWithTypeArguments(Object... args){
			return new Error("compiler","cannot.create.array.with.type.arguments",args);
		}

		public static final Error CannotCreateArrayWithTypeArguments = new Error("compiler","cannot.create.array.with.type.arguments");

		public static Error NotDefPublicCantAccess(Object... args){
			return new Error("compiler","not.def.public.cant.access",args);
		}

		public static Error DcUnterminatedString(Object... args){
			return new Error("compiler","dc.unterminated.string",args);
		}

		public static final Error DcUnterminatedString = new Error("compiler","dc.unterminated.string");

		public static Error ModNotAllowedHere(Object... args){
			return new Error("compiler","mod.not.allowed.here",args);
		}

		public static Error CantResolveArgsParams(Object... args){
			return new Error("compiler","cant.resolve.args.params",args);
		}

		public static Error InvalidRepeatableAnnotationMultipleValues(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.multiple.values",args);
		}

		public static Error CantResolve(Object... args){
			return new Error("compiler","cant.resolve",args);
		}

		public static Error VarNotAllowedCompound(Object... args){
			return new Error("compiler","var.not.allowed.compound",args);
		}

		public static final Error VarNotAllowedCompound = new Error("compiler","var.not.allowed.compound");

		public static Error IntfExpectedHere(Object... args){
			return new Error("compiler","intf.expected.here",args);
		}

		public static final Error IntfExpectedHere = new Error("compiler","intf.expected.here");

		public static Error ConstExprReq(Object... args){
			return new Error("compiler","const.expr.req",args);
		}

		public static final Error ConstExprReq = new Error("compiler","const.expr.req");

		public static Error DuplicateAnnotationInvalidRepeated(Object... args){
			return new Error("compiler","duplicate.annotation.invalid.repeated",args);
		}

		public static Error LimitLocals(Object... args){
			return new Error("compiler","limit.locals",args);
		}

		public static final Error LimitLocals = new Error("compiler","limit.locals");

		public static Error NotDefAccessPackageCantAccess(Object... args){
			return new Error("compiler","not.def.access.package.cant.access",args);
		}

		public static Error ArrayAndVarargs(Object... args){
			return new Error("compiler","array.and.varargs",args);
		}

		public static Error NotEnclClass(Object... args){
			return new Error("compiler","not.encl.class",args);
		}

		public static Error InvalidMref(Object... args){
			return new Error("compiler","invalid.mref",args);
		}

		public static Error TypeVarMayNotBeFollowedByOtherBounds(Object... args){
			return new Error("compiler","type.var.may.not.be.followed.by.other.bounds",args);
		}

		public static final Error TypeVarMayNotBeFollowedByOtherBounds = new Error("compiler","type.var.may.not.be.followed.by.other.bounds");

		public static Error ArrayDimensionMissing(Object... args){
			return new Error("compiler","array.dimension.missing",args);
		}

		public static final Error ArrayDimensionMissing = new Error("compiler","array.dimension.missing");

		public static Error ClassNotAllowed(Object... args){
			return new Error("compiler","class.not.allowed",args);
		}

		public static final Error ClassNotAllowed = new Error("compiler","class.not.allowed");

		public static Error AnnotationMissingDefaultValue(Object... args){
			return new Error("compiler","annotation.missing.default.value",args);
		}

		public static Error DuplicateUses(Object... args){
			return new Error("compiler","duplicate.uses",args);
		}

		public static Error AnonClassImplIntfNoArgs(Object... args){
			return new Error("compiler","anon.class.impl.intf.no.args",args);
		}

		public static final Error AnonClassImplIntfNoArgs = new Error("compiler","anon.class.impl.intf.no.args");

		public static Error Expected2(Object... args){
			return new Error("compiler","expected2",args);
		}

		public static Error IllegalArrayCreationBothDimensionAndInitialization(Object... args){
			return new Error("compiler","illegal.array.creation.both.dimension.and.initialization",args);
		}

		public static final Error IllegalArrayCreationBothDimensionAndInitialization = new Error("compiler","illegal.array.creation.both.dimension.and.initialization");

		public static Error Expected3(Object... args){
			return new Error("compiler","expected3",args);
		}

		public static Error AbstractMethCantHaveBody(Object... args){
			return new Error("compiler","abstract.meth.cant.have.body",args);
		}

		public static final Error AbstractMethCantHaveBody = new Error("compiler","abstract.meth.cant.have.body");

		public static Error ProcCantAccess1(Object... args){
			return new Error("compiler","proc.cant.access.1",args);
		}

		public static Error IllegalInitializerForType(Object... args){
			return new Error("compiler","illegal.initializer.for.type",args);
		}

		public static Error CantInheritDiffArg(Object... args){
			return new Error("compiler","cant.inherit.diff.arg",args);
		}

		public static Error NoPkgInModuleInfoJava(Object... args){
			return new Error("compiler","no.pkg.in.module-info.java",args);
		}

		public static final Error NoPkgInModuleInfoJava = new Error("compiler","no.pkg.in.module-info.java");

		public static Error IncorrectConstructorReceiverType(Object... args){
			return new Error("compiler","incorrect.constructor.receiver.type",args);
		}

		public static Error ModuleNotFoundInModuleSourcePath(Object... args){
			return new Error("compiler","module.not.found.in.module.source.path",args);
		}

		public static Error UnderscoreAsIdentifier(Object... args){
			return new Error("compiler","underscore.as.identifier",args);
		}

		public static final Error UnderscoreAsIdentifier = new Error("compiler","underscore.as.identifier");

		public static Error UnsupportedCrossFpLit(Object... args){
			return new Error("compiler","unsupported.cross.fp.lit",args);
		}

		public static final Error UnsupportedCrossFpLit = new Error("compiler","unsupported.cross.fp.lit");

		public static Error SyntheticNameConflict(Object... args){
			return new Error("compiler","synthetic.name.conflict",args);
		}

		public static Error OptionRemovedTarget(Object... args){
			return new Error("compiler","option.removed.target",args);
		}

		public static Error CantRefBeforeCtorCalled(Object... args){
			return new Error("compiler","cant.ref.before.ctor.called",args);
		}

		public static Error InvalidRepeatableAnnotationNotApplicableInContext(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.not.applicable.in.context",args);
		}

		public static Error AnnotationValueMustBeAnnotation(Object... args){
			return new Error("compiler","annotation.value.must.be.annotation",args);
		}

		public static final Error AnnotationValueMustBeAnnotation = new Error("compiler","annotation.value.must.be.annotation");

		public static Error IllegalUnicodeEsc(Object... args){
			return new Error("compiler","illegal.unicode.esc",args);
		}

		public static final Error IllegalUnicodeEsc = new Error("compiler","illegal.unicode.esc");

		public static Error CantResolveArgs(Object... args){
			return new Error("compiler","cant.resolve.args",args);
		}

		public static Error TypesIncompatibleAbstractDefault(Object... args){
			return new Error("compiler","types.incompatible.abstract.default",args);
		}

		public static Error DuplicateProvides(Object... args){
			return new Error("compiler","duplicate.provides",args);
		}

		public static Error ServiceImplementationProviderReturnMustBeSubtypeOfServiceInterface(Object... args){
			return new Error("compiler","service.implementation.provider.return.must.be.subtype.of.service.interface",args);
		}

		public static final Error ServiceImplementationProviderReturnMustBeSubtypeOfServiceInterface = new Error("compiler","service.implementation.provider.return.must.be.subtype.of.service.interface");

		public static Error DcMissingSemicolon(Object... args){
			return new Error("compiler","dc.missing.semicolon",args);
		}

		public static final Error DcMissingSemicolon = new Error("compiler","dc.missing.semicolon");

		public static Error UnsupportedBinaryLit(Object... args){
			return new Error("compiler","unsupported.binary.lit",args);
		}

		public static Error CantResolveLocation(Object... args){
			return new Error("compiler","cant.resolve.location",args);
		}

		public static Error IllegalUnderscore(Object... args){
			return new Error("compiler","illegal.underscore",args);
		}

		public static final Error IllegalUnderscore = new Error("compiler","illegal.underscore");

		public static Error CantAssignValToThis(Object... args){
			return new Error("compiler","cant.assign.val.to.this",args);
		}

		public static final Error CantAssignValToThis = new Error("compiler","cant.assign.val.to.this");

		public static Error CantApplySymbols(Object... args){
			return new Error("compiler","cant.apply.symbols",args);
		}

		public static Error VarNotInitializedInDefaultConstructor(Object... args){
			return new Error("compiler","var.not.initialized.in.default.constructor",args);
		}

		public static Error VarargsInvalidTrustmeAnno(Object... args){
			return new Error("compiler","varargs.invalid.trustme.anno",args);
		}

		public static Error ModulesNotSupportedInSource(Object... args){
			return new Error("compiler","modules.not.supported.in.source",args);
		}

		public static Error LimitCode(Object... args){
			return new Error("compiler","limit.code",args);
		}

		public static final Error LimitCode = new Error("compiler","limit.code");

		public static Error InvalidRepeatableAnnotationNotDocumented(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.not.documented",args);
		}

		public static Error VarMightNotHaveBeenInitialized(Object... args){
			return new Error("compiler","var.might.not.have.been.initialized",args);
		}

		public static Error SignatureDoesntMatchSupertype(Object... args){
			return new Error("compiler","signature.doesnt.match.supertype",args);
		}

		public static Error IllegalRefToVarType(Object... args){
			return new Error("compiler","illegal.ref.to.var.type",args);
		}

		public static Error UnexpectedLambda(Object... args){
			return new Error("compiler","unexpected.lambda",args);
		}

		public static final Error UnexpectedLambda = new Error("compiler","unexpected.lambda");

		public static Error ConcreteInheritanceConflict(Object... args){
			return new Error("compiler","concrete.inheritance.conflict",args);
		}

		public static Error LocnModuleInfoNotAllowedOnPatchPath(Object... args){
			return new Error("compiler","locn.module-info.not.allowed.on.patch.path",args);
		}

		public static Error ArrayReqButFound(Object... args){
			return new Error("compiler","array.req.but.found",args);
		}

		public static Error ClassCantWrite(Object... args){
			return new Error("compiler","class.cant.write",args);
		}

		public static Error LimitCodeTooLargeForTryStmt(Object... args){
			return new Error("compiler","limit.code.too.large.for.try.stmt",args);
		}

		public static final Error LimitCodeTooLargeForTryStmt = new Error("compiler","limit.code.too.large.for.try.stmt");

		public static Error NotAnnotationType(Object... args){
			return new Error("compiler","not.annotation.type",args);
		}

		public static Error ServiceImplementationMustBeSubtypeOfServiceInterface(Object... args){
			return new Error("compiler","service.implementation.must.be.subtype.of.service.interface",args);
		}

		public static final Error ServiceImplementationMustBeSubtypeOfServiceInterface = new Error("compiler","service.implementation.must.be.subtype.of.service.interface");

		public static Error DcUnexpectedContent(Object... args){
			return new Error("compiler","dc.unexpected.content",args);
		}

		public static final Error DcUnexpectedContent = new Error("compiler","dc.unexpected.content");

		public static Error LocnCantGetModuleNameForJar(Object... args){
			return new Error("compiler","locn.cant.get.module.name.for.jar",args);
		}

		public static Error IllegalEnumStaticRef(Object... args){
			return new Error("compiler","illegal.enum.static.ref",args);
		}

		public static final Error IllegalEnumStaticRef = new Error("compiler","illegal.enum.static.ref");

		public static Error PackageClashFromRequiresInUnnamed(Object... args){
			return new Error("compiler","package.clash.from.requires.in.unnamed",args);
		}

		public static Error RepeatedModifier(Object... args){
			return new Error("compiler","repeated.modifier",args);
		}

		public static final Error RepeatedModifier = new Error("compiler","repeated.modifier");

		public static Error AbstractCantBeAccessedDirectly(Object... args){
			return new Error("compiler","abstract.cant.be.accessed.directly",args);
		}

		public static Error DcUnterminatedSignature(Object... args){
			return new Error("compiler","dc.unterminated.signature",args);
		}

		public static final Error DcUnterminatedSignature = new Error("compiler","dc.unterminated.signature");

		public static Error SignatureDoesntMatchIntf(Object... args){
			return new Error("compiler","signature.doesnt.match.intf",args);
		}

		public static Error DuplicateModule(Object... args){
			return new Error("compiler","duplicate.module",args);
		}

		public static Error NoIntfExpectedHere(Object... args){
			return new Error("compiler","no.intf.expected.here",args);
		}

		public static final Error NoIntfExpectedHere = new Error("compiler","no.intf.expected.here");

		public static Error DcUnterminatedInlineTag(Object... args){
			return new Error("compiler","dc.unterminated.inline.tag",args);
		}

		public static final Error DcUnterminatedInlineTag = new Error("compiler","dc.unterminated.inline.tag");

		public static Error UnderscoreAsIdentifierInLambda(Object... args){
			return new Error("compiler","underscore.as.identifier.in.lambda",args);
		}

		public static final Error UnderscoreAsIdentifierInLambda = new Error("compiler","underscore.as.identifier.in.lambda");

		public static Error IllegalStartOfStmt(Object... args){
			return new Error("compiler","illegal.start.of.stmt",args);
		}

		public static final Error IllegalStartOfStmt = new Error("compiler","illegal.start.of.stmt");

		public static Error ProcCantFindClass(Object... args){
			return new Error("compiler","proc.cant.find.class",args);
		}

		public static Error ReceiverParameterNotApplicableConstructorToplevelClass(Object... args){
			return new Error("compiler","receiver.parameter.not.applicable.constructor.toplevel.class",args);
		}

		public static final Error ReceiverParameterNotApplicableConstructorToplevelClass = new Error("compiler","receiver.parameter.not.applicable.constructor.toplevel.class");

		public static Error DotClassExpected(Object... args){
			return new Error("compiler","dot.class.expected",args);
		}

		public static final Error DotClassExpected = new Error("compiler","dot.class.expected");

		public static Error AnnotationsAfterTypeParamsNotSupportedInSource(Object... args){
			return new Error("compiler","annotations.after.type.params.not.supported.in.source",args);
		}

		public static Error BadNameForOption(Object... args){
			return new Error("compiler","bad.name.for.option",args);
		}

		public static Error CallToSuperNotAllowedInEnumCtor(Object... args){
			return new Error("compiler","call.to.super.not.allowed.in.enum.ctor",args);
		}

		public static final Error CallToSuperNotAllowedInEnumCtor = new Error("compiler","call.to.super.not.allowed.in.enum.ctor");

		public static Error NotDefPublic(Object... args){
			return new Error("compiler","not.def.public",args);
		}

		public static Error DcBadGt(Object... args){
			return new Error("compiler","dc.bad.gt",args);
		}

		public static final Error DcBadGt = new Error("compiler","dc.bad.gt");

		public static Error IllegalSelfRef(Object... args){
			return new Error("compiler","illegal.self.ref",args);
		}

		public static final Error IllegalSelfRef = new Error("compiler","illegal.self.ref");

		public static Error ProcProcessorBadOptionName(Object... args){
			return new Error("compiler","proc.processor.bad.option.name",args);
		}

		public static Error TypesIncompatibleDiffRet(Object... args){
			return new Error("compiler","types.incompatible.diff.ret",args);
		}

		public static Error ExpressionNotAllowableAsAnnotationValue(Object... args){
			return new Error("compiler","expression.not.allowable.as.annotation.value",args);
		}

		public static final Error ExpressionNotAllowableAsAnnotationValue = new Error("compiler","expression.not.allowable.as.annotation.value");

		public static Error NoSuperclass(Object... args){
			return new Error("compiler","no.superclass",args);
		}

		public static Error InvalidRepeatableAnnotationNotInherited(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.not.inherited",args);
		}

		public static Error TypeAnnotationsNotSupportedInSource(Object... args){
			return new Error("compiler","type.annotations.not.supported.in.source",args);
		}

		public static Error CantTypeAnnotateScoping(Object... args){
			return new Error("compiler","cant.type.annotate.scoping",args);
		}

		public static Error MulticatchParameterMayNotBeAssigned(Object... args){
			return new Error("compiler","multicatch.parameter.may.not.be.assigned",args);
		}

		public static Error BadInitializer(Object... args){
			return new Error("compiler","bad.initializer",args);
		}

		public static Error UnnamedPkgNotAllowedNamedModules(Object... args){
			return new Error("compiler","unnamed.pkg.not.allowed.named.modules",args);
		}

		public static final Error UnnamedPkgNotAllowedNamedModules = new Error("compiler","unnamed.pkg.not.allowed.named.modules");

		public static Error AnonClassImplIntfNoQualForNew(Object... args){
			return new Error("compiler","anon.class.impl.intf.no.qual.for.new",args);
		}

		public static final Error AnonClassImplIntfNoQualForNew = new Error("compiler","anon.class.impl.intf.no.qual.for.new");

		public static Error ModuleNotFound(Object... args){
			return new Error("compiler","module.not.found",args);
		}

		public static Error FinalParameterMayNotBeAssigned(Object... args){
			return new Error("compiler","final.parameter.may.not.be.assigned",args);
		}

		public static Error VarargsAndReceiver(Object... args){
			return new Error("compiler","varargs.and.receiver",args);
		}

		public static final Error VarargsAndReceiver = new Error("compiler","varargs.and.receiver");

		public static Error NameClashSameErasure(Object... args){
			return new Error("compiler","name.clash.same.erasure",args);
		}

		public static Error DcNoTagName(Object... args){
			return new Error("compiler","dc.no.tag.name",args);
		}

		public static final Error DcNoTagName = new Error("compiler","dc.no.tag.name");

		public static Error DuplicateClass(Object... args){
			return new Error("compiler","duplicate.class",args);
		}

		public static Error CantReadFile(Object... args){
			return new Error("compiler","cant.read.file",args);
		}

		public static Error EnumTypesNotExtensible(Object... args){
			return new Error("compiler","enum.types.not.extensible",args);
		}

		public static final Error EnumTypesNotExtensible = new Error("compiler","enum.types.not.extensible");

		public static Error VariableNotAllowed(Object... args){
			return new Error("compiler","variable.not.allowed",args);
		}

		public static final Error VariableNotAllowed = new Error("compiler","variable.not.allowed");

		public static Error DcRefBadParens(Object... args){
			return new Error("compiler","dc.ref.bad.parens",args);
		}

		public static final Error DcRefBadParens = new Error("compiler","dc.ref.bad.parens");

		public static Error GenericArrayCreation(Object... args){
			return new Error("compiler","generic.array.creation",args);
		}

		public static final Error GenericArrayCreation = new Error("compiler","generic.array.creation");

		public static Error ErrorReadingFile(Object... args){
			return new Error("compiler","error.reading.file",args);
		}

		public static Error PrematureEof(Object... args){
			return new Error("compiler","premature.eof",args);
		}

		public static final Error PrematureEof = new Error("compiler","premature.eof");

		public static Error UnexpectedType(Object... args){
			return new Error("compiler","unexpected.type",args);
		}

		public static Error AlreadyDefinedInClinit(Object... args){
			return new Error("compiler","already.defined.in.clinit",args);
		}

		public static Error EnumAnnotationMustBeEnumConstant(Object... args){
			return new Error("compiler","enum.annotation.must.be.enum.constant",args);
		}

		public static final Error EnumAnnotationMustBeEnumConstant = new Error("compiler","enum.annotation.must.be.enum.constant");

		public static Error PackageNotVisible(Object... args){
			return new Error("compiler","package.not.visible",args);
		}

		public static Error NoMatchEntry(Object... args){
			return new Error("compiler","no.match.entry",args);
		}

		public static Error TryWithResourcesExprNeedsVar(Object... args){
			return new Error("compiler","try.with.resources.expr.needs.var",args);
		}

		public static final Error TryWithResourcesExprNeedsVar = new Error("compiler","try.with.resources.expr.needs.var");

		public static Error IllegalParenthesizedExpression(Object... args){
			return new Error("compiler","illegal.parenthesized.expression",args);
		}

		public static final Error IllegalParenthesizedExpression = new Error("compiler","illegal.parenthesized.expression");

		public static Error TryWithoutCatchOrFinally(Object... args){
			return new Error("compiler","try.without.catch.or.finally",args);
		}

		public static final Error TryWithoutCatchOrFinally = new Error("compiler","try.without.catch.or.finally");

		public static Error ProcCantCreateLoader(Object... args){
			return new Error("compiler","proc.cant.create.loader",args);
		}

		public static Error ProcProcessorCantInstantiate(Object... args){
			return new Error("compiler","proc.processor.cant.instantiate",args);
		}

		public static Error IntfAnnotationMembersCantHaveParams(Object... args){
			return new Error("compiler","intf.annotation.members.cant.have.params",args);
		}

		public static final Error IntfAnnotationMembersCantHaveParams = new Error("compiler","intf.annotation.members.cant.have.params");

		public static Error CannotCreateArrayWithDiamond(Object... args){
			return new Error("compiler","cannot.create.array.with.diamond",args);
		}

		public static final Error CannotCreateArrayWithDiamond = new Error("compiler","cannot.create.array.with.diamond");

		public static Error InvalidRepeatableAnnotationElemNondefault(Object... args){
			return new Error("compiler","invalid.repeatable.annotation.elem.nondefault",args);
		}

	}
	public static class Fragments {
		public static Fragment NotDefAccessNotExportedToModuleFromUnnamed(Object... args){
			return new Fragment("compiler","not.def.access.not.exported.to.module.from.unnamed",args);
		}

		public static Fragment VarargsArgumentMismatch(Object... args){
			return new Fragment("compiler","varargs.argument.mismatch",args);
		}

		public static Fragment ProbFoundReq(Object... args){
			return new Fragment("compiler","prob.found.req",args);
		}

		public static Fragment BadClassFileHeader(Object... args){
			return new Fragment("compiler","bad.class.file.header",args);
		}

		public static Fragment IntersectionType(Object... args){
			return new Fragment("compiler","intersection.type",args);
		}

		public static Fragment TokenEndOfInput(Object... args){
			return new Fragment("compiler","token.end-of-input",args);
		}

		public static final Fragment TokenEndOfInput = new Fragment("compiler","token.end-of-input");

		public static Fragment CantHide(Object... args){
			return new Fragment("compiler","cant.hide",args);
		}

		public static Fragment InferredDoNotConformToLowerBounds(Object... args){
			return new Fragment("compiler","inferred.do.not.conform.to.lower.bounds",args);
		}

		public static Fragment TypeReqExact(Object... args){
			return new Fragment("compiler","type.req.exact",args);
		}

		public static final Fragment TypeReqExact = new Fragment("compiler","type.req.exact");

		public static Fragment LocalMissingInit(Object... args){
			return new Fragment("compiler","local.missing.init",args);
		}

		public static final Fragment LocalMissingInit = new Fragment("compiler","local.missing.init");

		public static Fragment IncompatibleAbstracts(Object... args){
			return new Fragment("compiler","incompatible.abstracts",args);
		}

		public static Fragment CantResolveLocationArgs(Object... args){
			return new Fragment("compiler","cant.resolve.location.args",args);
		}

		public static Fragment ClassFileNotFound(Object... args){
			return new Fragment("compiler","class.file.not.found",args);
		}

		public static Fragment CantResolveLocationArgsParams(Object... args){
			return new Fragment("compiler","cant.resolve.location.args.params",args);
		}

		public static Fragment UncheckedImplement(Object... args){
			return new Fragment("compiler","unchecked.implement",args);
		}

		public static Fragment WhereCaptured(Object... args){
			return new Fragment("compiler","where.captured",args);
		}

		public static Fragment FileDoesntContainClass(Object... args){
			return new Fragment("compiler","file.doesnt.contain.class",args);
		}

		public static Fragment LocalMrefMissingTarget(Object... args){
			return new Fragment("compiler","local.mref.missing.target",args);
		}

		public static final Fragment LocalMrefMissingTarget = new Fragment("compiler","local.mref.missing.target");

		public static Fragment MrefInferAndExplicitParams(Object... args){
			return new Fragment("compiler","mref.infer.and.explicit.params",args);
		}

		public static final Fragment MrefInferAndExplicitParams = new Fragment("compiler","mref.infer.and.explicit.params");

		public static Fragment IncompatibleUpperLowerBounds(Object... args){
			return new Fragment("compiler","incompatible.upper.lower.bounds",args);
		}

		public static Fragment VarargsOverride(Object... args){
			return new Fragment("compiler","varargs.override",args);
		}

		public static Fragment NotAnIntfComponent(Object... args){
			return new Fragment("compiler","not.an.intf.component",args);
		}

		public static Fragment InaccessibleVarargsType(Object... args){
			return new Fragment("compiler","inaccessible.varargs.type",args);
		}

		public static Fragment UserSelectedCompletionFailure(Object... args){
			return new Fragment("compiler","user.selected.completion.failure",args);
		}

		public static final Fragment UserSelectedCompletionFailure = new Fragment("compiler","user.selected.completion.failure");

		public static Fragment NotApplicableMethodFound(Object... args){
			return new Fragment("compiler","not.applicable.method.found",args);
		}

		public static Fragment Descriptor(Object... args){
			return new Fragment("compiler","descriptor",args);
		}

		public static Fragment StatExprExpected(Object... args){
			return new Fragment("compiler","stat.expr.expected",args);
		}

		public static final Fragment StatExprExpected = new Fragment("compiler","stat.expr.expected");

		public static Fragment InferArgLengthMismatch(Object... args){
			return new Fragment("compiler","infer.arg.length.mismatch",args);
		}

		public static Fragment NoSuitableFunctionalIntfInst(Object... args){
			return new Fragment("compiler","no.suitable.functional.intf.inst",args);
		}

		public static Fragment TypeReqClassArray(Object... args){
			return new Fragment("compiler","type.req.class.array",args);
		}

		public static final Fragment TypeReqClassArray = new Fragment("compiler","type.req.class.array");

		public static Fragment NotDefAccessDoesNotReadFromUnnamed(Object... args){
			return new Fragment("compiler","not.def.access.does.not.read.from.unnamed",args);
		}

		public static Fragment WrongNumberTypeArgs(Object... args){
			return new Fragment("compiler","wrong.number.type.args",args);
		}

		public static Fragment NoAbstracts(Object... args){
			return new Fragment("compiler","no.abstracts",args);
		}

		public static Fragment InferVarargsArgumentMismatch(Object... args){
			return new Fragment("compiler","infer.varargs.argument.mismatch",args);
		}

		public static Fragment BadConstantRange(Object... args){
			return new Fragment("compiler","bad.constant.range",args);
		}

		public static Fragment LocnModule_path(Object... args){
			return new Fragment("compiler","locn.module_path",args);
		}

		public static final Fragment LocnModule_path = new Fragment("compiler","locn.module_path");

		public static Fragment Diamond(Object... args){
			return new Fragment("compiler","diamond",args);
		}

		public static Fragment BaseMembership(Object... args){
			return new Fragment("compiler","base.membership",args);
		}

		public static final Fragment BaseMembership = new Fragment("compiler","base.membership");

		public static Fragment LocnUpgrade_module_path(Object... args){
			return new Fragment("compiler","locn.upgrade_module_path",args);
		}

		public static final Fragment LocnUpgrade_module_path = new Fragment("compiler","locn.upgrade_module_path");

		public static Fragment TypeNull(Object... args){
			return new Fragment("compiler","type.null",args);
		}

		public static final Fragment TypeNull = new Fragment("compiler","type.null");

		public static Fragment ApplicableMethodFound(Object... args){
			return new Fragment("compiler","applicable.method.found",args);
		}

		public static Fragment XPrintRounds(Object... args){
			return new Fragment("compiler","x.print.rounds",args);
		}

		public static Fragment InferredDoNotConformToEqBounds(Object... args){
			return new Fragment("compiler","inferred.do.not.conform.to.eq.bounds",args);
		}

		public static Fragment IncompatibleArgTypesInLambda(Object... args){
			return new Fragment("compiler","incompatible.arg.types.in.lambda",args);
		}

		public static final Fragment IncompatibleArgTypesInLambda = new Fragment("compiler","incompatible.arg.types.in.lambda");

		public static Fragment FatalErrCantClose(Object... args){
			return new Fragment("compiler","fatal.err.cant.close",args);
		}

		public static final Fragment FatalErrCantClose = new Fragment("compiler","fatal.err.cant.close");

		public static Fragment Lambda(Object... args){
			return new Fragment("compiler","lambda",args);
		}

		public static final Fragment Lambda = new Fragment("compiler","lambda");

		public static Fragment CountWarn(Object... args){
			return new Fragment("compiler","count.warn",args);
		}

		public static Fragment ArgLengthMismatch(Object... args){
			return new Fragment("compiler","arg.length.mismatch",args);
		}

		public static final Fragment ArgLengthMismatch = new Fragment("compiler","arg.length.mismatch");

		public static Fragment WhereDescriptionIntersection1(Object... args){
			return new Fragment("compiler","where.description.intersection.1",args);
		}

		public static Fragment IncompatibleRetTypeInMref(Object... args){
			return new Fragment("compiler","incompatible.ret.type.in.mref",args);
		}

		public static Fragment DiamondAndExplicitParams(Object... args){
			return new Fragment("compiler","diamond.and.explicit.params",args);
		}

		public static final Fragment DiamondAndExplicitParams = new Fragment("compiler","diamond.and.explicit.params");

		public static Fragment IncompatibleUpperBounds(Object... args){
			return new Fragment("compiler","incompatible.upper.bounds",args);
		}

		public static Fragment WhereDescriptionTypevar1(Object... args){
			return new Fragment("compiler","where.description.typevar.1",args);
		}

		public static Fragment BadClassSignature(Object... args){
			return new Fragment("compiler","bad.class.signature",args);
		}

		public static Fragment WhereDescriptionTypevar(Object... args){
			return new Fragment("compiler","where.description.typevar",args);
		}

		public static Fragment BadEnclosingMethod(Object... args){
			return new Fragment("compiler","bad.enclosing.method",args);
		}

		public static Fragment TypeVar(Object... args){
			return new Fragment("compiler","type.var",args);
		}

		public static Fragment KindnameVariable(Object... args){
			return new Fragment("compiler","kindname.variable",args);
		}

		public static final Fragment KindnameVariable = new Fragment("compiler","kindname.variable");

		public static Fragment InapplicableMethod(Object... args){
			return new Fragment("compiler","inapplicable.method",args);
		}

		public static Fragment RedundantSupertype(Object... args){
			return new Fragment("compiler","redundant.supertype",args);
		}

		public static Fragment RefAmbiguous(Object... args){
			return new Fragment("compiler","ref.ambiguous",args);
		}

		public static Fragment BadSourceFileHeader(Object... args){
			return new Fragment("compiler","bad.source.file.header",args);
		}

		public static Fragment TypeReqArrayOrIterable(Object... args){
			return new Fragment("compiler","type.req.array.or.iterable",args);
		}

		public static final Fragment TypeReqArrayOrIterable = new Fragment("compiler","type.req.array.or.iterable");

		public static Fragment KindnameInstanceInit(Object... args){
			return new Fragment("compiler","kindname.instance.init",args);
		}

		public static final Fragment KindnameInstanceInit = new Fragment("compiler","kindname.instance.init");

		public static Fragment NoConformingAssignmentExists(Object... args){
			return new Fragment("compiler","no.conforming.assignment.exists",args);
		}

		public static Fragment UncheckedAssign(Object... args){
			return new Fragment("compiler","unchecked.assign",args);
		}

		public static final Fragment UncheckedAssign = new Fragment("compiler","unchecked.assign");

		public static Fragment VarargsTrustmeOnVirtualVarargs(Object... args){
			return new Fragment("compiler","varargs.trustme.on.virtual.varargs",args);
		}

		public static Fragment CountErrorPlural(Object... args){
			return new Fragment("compiler","count.error.plural",args);
		}

		public static Fragment BadRuntimeInvisibleParamAnnotations(Object... args){
			return new Fragment("compiler","bad.runtime.invisible.param.annotations",args);
		}

		public static Fragment WhereDescriptionCaptured1(Object... args){
			return new Fragment("compiler","where.description.captured.1",args);
		}

		public static Fragment InferredDoNotConformToUpperBounds(Object... args){
			return new Fragment("compiler","inferred.do.not.conform.to.upper.bounds",args);
		}

		public static Fragment LocalSelfRef(Object... args){
			return new Fragment("compiler","local.self.ref",args);
		}

		public static final Fragment LocalSelfRef = new Fragment("compiler","local.self.ref");

		public static Fragment MalformedVarargMethod(Object... args){
			return new Fragment("compiler","malformed.vararg.method",args);
		}

		public static Fragment BadModuleInfoName(Object... args){
			return new Fragment("compiler","bad.module-info.name",args);
		}

		public static final Fragment BadModuleInfoName = new Fragment("compiler","bad.module-info.name");

		public static Fragment BadStaticMethodInUnboundLookup(Object... args){
			return new Fragment("compiler","bad.static.method.in.unbound.lookup",args);
		}

		public static Fragment PossibleLossOfPrecision(Object... args){
			return new Fragment("compiler","possible.loss.of.precision",args);
		}

		public static Fragment IncompatibleTypeInConditional(Object... args){
			return new Fragment("compiler","incompatible.type.in.conditional",args);
		}

		public static Fragment LocalLambdaMissingTarget(Object... args){
			return new Fragment("compiler","local.lambda.missing.target",args);
		}

		public static final Fragment LocalLambdaMissingTarget = new Fragment("compiler","local.lambda.missing.target");

		public static Fragment TokenInteger(Object... args){
			return new Fragment("compiler","token.integer",args);
		}

		public static final Fragment TokenInteger = new Fragment("compiler","token.integer");

		public static Fragment UnicodeStrNotSupported(Object... args){
			return new Fragment("compiler","unicode.str.not.supported",args);
		}

		public static final Fragment UnicodeStrNotSupported = new Fragment("compiler","unicode.str.not.supported");

		public static Fragment VarargsTrustmeOnReifiableVarargs(Object... args){
			return new Fragment("compiler","varargs.trustme.on.reifiable.varargs",args);
		}

		public static Fragment KindnameInterface(Object... args){
			return new Fragment("compiler","kindname.interface",args);
		}

		public static final Fragment KindnameInterface = new Fragment("compiler","kindname.interface");

		public static Fragment CantResolveModules(Object... args){
			return new Fragment("compiler","cant.resolve.modules",args);
		}

		public static final Fragment CantResolveModules = new Fragment("compiler","cant.resolve.modules");

		public static Fragment FatalErrCantLocateCtor(Object... args){
			return new Fragment("compiler","fatal.err.cant.locate.ctor",args);
		}

		public static Fragment FatalErrCantLocateMeth(Object... args){
			return new Fragment("compiler","fatal.err.cant.locate.meth",args);
		}

		public static Fragment CountWarnPlural(Object... args){
			return new Fragment("compiler","count.warn.plural",args);
		}

		public static Fragment KindnameMethod(Object... args){
			return new Fragment("compiler","kindname.method",args);
		}

		public static final Fragment KindnameMethod = new Fragment("compiler","kindname.method");

		public static Fragment ConditionalTargetCantBeVoid(Object... args){
			return new Fragment("compiler","conditional.target.cant.be.void",args);
		}

		public static final Fragment ConditionalTargetCantBeVoid = new Fragment("compiler","conditional.target.cant.be.void");

		public static Fragment ClashesWith(Object... args){
			return new Fragment("compiler","clashes.with",args);
		}

		public static Fragment TokenBadSymbol(Object... args){
			return new Fragment("compiler","token.bad-symbol",args);
		}

		public static final Fragment TokenBadSymbol = new Fragment("compiler","token.bad-symbol");

		public static Fragment BadSignature(Object... args){
			return new Fragment("compiler","bad.signature",args);
		}

		public static Fragment ExplicitParamDoNotConformToBounds(Object... args){
			return new Fragment("compiler","explicit.param.do.not.conform.to.bounds",args);
		}

		public static Fragment UnexpectedRetVal(Object... args){
			return new Fragment("compiler","unexpected.ret.val",args);
		}

		public static final Fragment UnexpectedRetVal = new Fragment("compiler","unexpected.ret.val");

		public static Fragment VerboseLoading(Object... args){
			return new Fragment("compiler","verbose.loading",args);
		}

		public static Fragment IncompatibleDescsInFunctionalIntf(Object... args){
			return new Fragment("compiler","incompatible.descs.in.functional.intf",args);
		}

		public static Fragment NotDefAccessClassIntfCantAccessReason(Object... args){
			return new Fragment("compiler","not.def.access.class.intf.cant.access.reason",args);
		}

		public static Fragment Location1(Object... args){
			return new Fragment("compiler","location.1",args);
		}

		public static Fragment BadTypeAnnotationValue(Object... args){
			return new Fragment("compiler","bad.type.annotation.value",args);
		}

		public static Fragment UncheckedCastToType(Object... args){
			return new Fragment("compiler","unchecked.cast.to.type",args);
		}

		public static final Fragment UncheckedCastToType = new Fragment("compiler","unchecked.cast.to.type");

		public static Fragment NotAFunctionalIntf(Object... args){
			return new Fragment("compiler","not.a.functional.intf",args);
		}

		public static Fragment LocnSystem_modules(Object... args){
			return new Fragment("compiler","locn.system_modules",args);
		}

		public static final Fragment LocnSystem_modules = new Fragment("compiler","locn.system_modules");

		public static Fragment FileDoesNotContainModule(Object... args){
			return new Fragment("compiler","file.does.not.contain.module",args);
		}

		public static final Fragment FileDoesNotContainModule = new Fragment("compiler","file.does.not.contain.module");

		public static Fragment WhereDescriptionCaptured(Object... args){
			return new Fragment("compiler","where.description.captured",args);
		}

		public static Fragment TokenString(Object... args){
			return new Fragment("compiler","token.string",args);
		}

		public static final Fragment TokenString = new Fragment("compiler","token.string");

		public static Fragment IncompatibleEqBounds(Object... args){
			return new Fragment("compiler","incompatible.eq.bounds",args);
		}

		public static Fragment VarargsImplement(Object... args){
			return new Fragment("compiler","varargs.implement",args);
		}

		public static Fragment DiamondAndAnonClassNotSupportedInSource(Object... args){
			return new Fragment("compiler","diamond.and.anon.class.not.supported.in.source",args);
		}

		public static Fragment VarargsTrustmeOnNonVarargsMeth(Object... args){
			return new Fragment("compiler","varargs.trustme.on.non.varargs.meth",args);
		}

		public static Fragment InvalidGenericLambdaTarget(Object... args){
			return new Fragment("compiler","invalid.generic.lambda.target",args);
		}

		public static Fragment BadConstPoolTag(Object... args){
			return new Fragment("compiler","bad.const.pool.tag",args);
		}

		public static Fragment ApplicableMethodFound1(Object... args){
			return new Fragment("compiler","applicable.method.found.1",args);
		}

		public static Fragment InferNoConformingInstanceExists(Object... args){
			return new Fragment("compiler","infer.no.conforming.instance.exists",args);
		}

		public static Fragment IncompatibleEqLowerBounds(Object... args){
			return new Fragment("compiler","incompatible.eq.lower.bounds",args);
		}

		public static Fragment TokenCharacter(Object... args){
			return new Fragment("compiler","token.character",args);
		}

		public static final Fragment TokenCharacter = new Fragment("compiler","token.character");

		public static Fragment NotDefAccessNotExportedFromUnnamed(Object... args){
			return new Fragment("compiler","not.def.access.not.exported.from.unnamed",args);
		}

		public static Fragment KindnameStaticInit(Object... args){
			return new Fragment("compiler","kindname.static.init",args);
		}

		public static final Fragment KindnameStaticInit = new Fragment("compiler","kindname.static.init");

		public static Fragment KindnamePackage(Object... args){
			return new Fragment("compiler","kindname.package",args);
		}

		public static final Fragment KindnamePackage = new Fragment("compiler","kindname.package");

		public static Fragment TypeCaptureof(Object... args){
			return new Fragment("compiler","type.captureof",args);
		}

		public static Fragment KindnameStatic(Object... args){
			return new Fragment("compiler","kindname.static",args);
		}

		public static final Fragment KindnameStatic = new Fragment("compiler","kindname.static");

		public static Fragment InvalidDefaultInterface(Object... args){
			return new Fragment("compiler","invalid.default.interface",args);
		}

		public static Fragment WhereFreshTypevar(Object... args){
			return new Fragment("compiler","where.fresh.typevar",args);
		}

		public static Fragment LocalArrayMissingTarget(Object... args){
			return new Fragment("compiler","local.array.missing.target",args);
		}

		public static final Fragment LocalArrayMissingTarget = new Fragment("compiler","local.array.missing.target");

		public static Fragment UnableToAccessFile(Object... args){
			return new Fragment("compiler","unable.to.access.file",args);
		}

		public static Fragment AnonymousClass(Object... args){
			return new Fragment("compiler","anonymous.class",args);
		}

		public static Fragment DiamondInvalidArgs(Object... args){
			return new Fragment("compiler","diamond.invalid.args",args);
		}

		public static Fragment BadInstanceMethodInUnboundLookup(Object... args){
			return new Fragment("compiler","bad.instance.method.in.unbound.lookup",args);
		}

		public static Fragment VerboseWroteFile(Object... args){
			return new Fragment("compiler","verbose.wrote.file",args);
		}

		public static Fragment NotDefAccessClassIntfCantAccess(Object... args){
			return new Fragment("compiler","not.def.access.class.intf.cant.access",args);
		}

		public static Fragment CountError(Object... args){
			return new Fragment("compiler","count.error",args);
		}

		public static Fragment IncompatibleRetTypeInLambda(Object... args){
			return new Fragment("compiler","incompatible.ret.type.in.lambda",args);
		}

		public static Fragment VerboseSourcepath(Object... args){
			return new Fragment("compiler","verbose.sourcepath",args);
		}

		public static Fragment SourceUnavailable(Object... args){
			return new Fragment("compiler","source.unavailable",args);
		}

		public static final Fragment SourceUnavailable = new Fragment("compiler","source.unavailable");

		public static Fragment NotDefAccessDoesNotRead(Object... args){
			return new Fragment("compiler","not.def.access.does.not.read",args);
		}

		public static Fragment IncompatibleArgTypesInMref(Object... args){
			return new Fragment("compiler","incompatible.arg.types.in.mref",args);
		}

		public static final Fragment IncompatibleArgTypesInMref = new Fragment("compiler","incompatible.arg.types.in.mref");

		public static Fragment TypeCaptureof1(Object... args){
			return new Fragment("compiler","type.captureof.1",args);
		}

		public static Fragment NotDefAccessNotExported(Object... args){
			return new Fragment("compiler","not.def.access.not.exported",args);
		}

		public static Fragment IncompatibleEqUpperBounds(Object... args){
			return new Fragment("compiler","incompatible.eq.upper.bounds",args);
		}

		public static Fragment VerboseParsingDone(Object... args){
			return new Fragment("compiler","verbose.parsing.done",args);
		}

		public static Fragment PartialInstSig(Object... args){
			return new Fragment("compiler","partial.inst.sig",args);
		}

		public static Fragment DiamondAnonymousMethodsImplicitlyOverride(Object... args){
			return new Fragment("compiler","diamond.anonymous.methods.implicitly.override",args);
		}

		public static final Fragment DiamondAnonymousMethodsImplicitlyOverride = new Fragment("compiler","diamond.anonymous.methods.implicitly.override");

		public static Fragment NotAFunctionalIntf1(Object... args){
			return new Fragment("compiler","not.a.functional.intf.1",args);
		}

		public static Fragment UncheckedOverride(Object... args){
			return new Fragment("compiler","unchecked.override",args);
		}

		public static Fragment UnnamedModule(Object... args){
			return new Fragment("compiler","unnamed.module",args);
		}

		public static final Fragment UnnamedModule = new Fragment("compiler","unnamed.module");

		public static Fragment BadEnclosingClass(Object... args){
			return new Fragment("compiler","bad.enclosing.class",args);
		}

		public static Fragment KindnameClass(Object... args){
			return new Fragment("compiler","kindname.class",args);
		}

		public static final Fragment KindnameClass = new Fragment("compiler","kindname.class");

		public static Fragment VersionNotAvailable(Object... args){
			return new Fragment("compiler","version.not.available",args);
		}

		public static final Fragment VersionNotAvailable = new Fragment("compiler","version.not.available");

		public static Fragment WhereIntersection(Object... args){
			return new Fragment("compiler","where.intersection",args);
		}

		public static Fragment UnnamedPackage(Object... args){
			return new Fragment("compiler","unnamed.package",args);
		}

		public static final Fragment UnnamedPackage = new Fragment("compiler","unnamed.package");

		public static Fragment ReportAccess(Object... args){
			return new Fragment("compiler","report.access",args);
		}

		public static Fragment ClassFileWrongClass(Object... args){
			return new Fragment("compiler","class.file.wrong.class",args);
		}

		public static Fragment IllegalStartOfClassFile(Object... args){
			return new Fragment("compiler","illegal.start.of.class.file",args);
		}

		public static final Fragment IllegalStartOfClassFile = new Fragment("compiler","illegal.start.of.class.file");

		public static Fragment WhereTypevar(Object... args){
			return new Fragment("compiler","where.typevar",args);
		}

		public static Fragment NoUniqueMinimalInstanceExists(Object... args){
			return new Fragment("compiler","no.unique.minimal.instance.exists",args);
		}

		public static Fragment KindnameEnum(Object... args){
			return new Fragment("compiler","kindname.enum",args);
		}

		public static final Fragment KindnameEnum = new Fragment("compiler","kindname.enum");

		public static Fragment KindnameAnnotation(Object... args){
			return new Fragment("compiler","kindname.annotation",args);
		}

		public static final Fragment KindnameAnnotation = new Fragment("compiler","kindname.annotation");

		public static Fragment ModuleInfoInvalidSuperClass(Object... args){
			return new Fragment("compiler","module.info.invalid.super.class",args);
		}

		public static final Fragment ModuleInfoInvalidSuperClass = new Fragment("compiler","module.info.invalid.super.class");

		public static Fragment OverriddenDefault(Object... args){
			return new Fragment("compiler","overridden.default",args);
		}

		public static Fragment BadConstPoolEntry(Object... args){
			return new Fragment("compiler","bad.const.pool.entry",args);
		}

		public static Fragment TypeParameter(Object... args){
			return new Fragment("compiler","type.parameter",args);
		}

		public static Fragment InferNoConformingAssignmentExists(Object... args){
			return new Fragment("compiler","infer.no.conforming.assignment.exists",args);
		}

		public static Fragment StaticMrefWithTargs(Object... args){
			return new Fragment("compiler","static.mref.with.targs",args);
		}

		public static final Fragment StaticMrefWithTargs = new Fragment("compiler","static.mref.with.targs");

		public static Fragment UndeclTypeVar(Object... args){
			return new Fragment("compiler","undecl.type.var",args);
		}

		public static Fragment VerboseCheckingAttribution(Object... args){
			return new Fragment("compiler","verbose.checking.attribution",args);
		}

		public static Fragment InvalidMref(Object... args){
			return new Fragment("compiler","invalid.mref",args);
		}

		public static Fragment ModuleNonZeroOpens(Object... args){
			return new Fragment("compiler","module.non.zero.opens",args);
		}

		public static Fragment TokenLongInteger(Object... args){
			return new Fragment("compiler","token.long-integer",args);
		}

		public static final Fragment TokenLongInteger = new Fragment("compiler","token.long-integer");

		public static Fragment TypeReqClass(Object... args){
			return new Fragment("compiler","type.req.class",args);
		}

		public static final Fragment TypeReqClass = new Fragment("compiler","type.req.class");

		public static Fragment AnachronisticModuleInfo(Object... args){
			return new Fragment("compiler","anachronistic.module.info",args);
		}

		public static Fragment TryNotApplicableToType(Object... args){
			return new Fragment("compiler","try.not.applicable.to.type",args);
		}

		public static Fragment FatalErrCantLocateField(Object... args){
			return new Fragment("compiler","fatal.err.cant.locate.field",args);
		}

		public static Fragment NoUniqueMaximalInstanceExists(Object... args){
			return new Fragment("compiler","no.unique.maximal.instance.exists",args);
		}

		public static Fragment TypeNone(Object... args){
			return new Fragment("compiler","type.none",args);
		}

		public static final Fragment TypeNone = new Fragment("compiler","type.none");

		public static Fragment InconvertibleTypes(Object... args){
			return new Fragment("compiler","inconvertible.types",args);
		}

		public static Fragment NotDefAccessNotExportedToModule(Object... args){
			return new Fragment("compiler","not.def.access.not.exported.to.module",args);
		}

		public static Fragment FileDoesNotContainPackage(Object... args){
			return new Fragment("compiler","file.does.not.contain.package",args);
		}

		public static Fragment CantApplySymbols(Object... args){
			return new Fragment("compiler","cant.apply.symbols",args);
		}

		public static Fragment UncheckedClashWith(Object... args){
			return new Fragment("compiler","unchecked.clash.with",args);
		}

		public static Fragment VarargsTrustmeOnVirtualVarargsFinalOnly(Object... args){
			return new Fragment("compiler","varargs.trustme.on.virtual.varargs.final.only",args);
		}

		public static Fragment NoArgs(Object... args){
			return new Fragment("compiler","no.args",args);
		}

		public static final Fragment NoArgs = new Fragment("compiler","no.args");

		public static Fragment CantAccessInnerClsConstr(Object... args){
			return new Fragment("compiler","cant.access.inner.cls.constr",args);
		}

		public static Fragment VarargsClashWith(Object... args){
			return new Fragment("compiler","varargs.clash.with",args);
		}

		public static Fragment WhereDescriptionIntersection(Object... args){
			return new Fragment("compiler","where.description.intersection",args);
		}

		public static Fragment CantOverride(Object... args){
			return new Fragment("compiler","cant.override",args);
		}

		public static Fragment KindnameConstructor(Object... args){
			return new Fragment("compiler","kindname.constructor",args);
		}

		public static final Fragment KindnameConstructor = new Fragment("compiler","kindname.constructor");

		public static Fragment Location(Object... args){
			return new Fragment("compiler","location",args);
		}

		public static Fragment DescriptorThrows(Object... args){
			return new Fragment("compiler","descriptor.throws",args);
		}

		public static Fragment CantImplement(Object... args){
			return new Fragment("compiler","cant.implement",args);
		}

		public static Fragment CantApplySymbol(Object... args){
			return new Fragment("compiler","cant.apply.symbol",args);
		}

		public static Fragment WhereTypevar1(Object... args){
			return new Fragment("compiler","where.typevar.1",args);
		}

		public static Fragment InnerCls(Object... args){
			return new Fragment("compiler","inner.cls",args);
		}

		public static final Fragment InnerCls = new Fragment("compiler","inner.cls");

		public static Fragment NotDefPublicCantAccess(Object... args){
			return new Fragment("compiler","not.def.public.cant.access",args);
		}

		public static Fragment XPrintProcessorInfo(Object... args){
			return new Fragment("compiler","x.print.processor.info",args);
		}

		public static Fragment ModuleNameMismatch(Object... args){
			return new Fragment("compiler","module.name.mismatch",args);
		}

		public static Fragment ResumeAbort(Object... args){
			return new Fragment("compiler","resume.abort",args);
		}

		public static final Fragment ResumeAbort = new Fragment("compiler","resume.abort");

		public static Fragment KindnameTypeVariable(Object... args){
			return new Fragment("compiler","kindname.type.variable",args);
		}

		public static final Fragment KindnameTypeVariable = new Fragment("compiler","kindname.type.variable");

		public static Fragment WhereCaptured1(Object... args){
			return new Fragment("compiler","where.captured.1",args);
		}

		public static Fragment LocalCantInferNull(Object... args){
			return new Fragment("compiler","local.cant.infer.null",args);
		}

		public static final Fragment LocalCantInferNull = new Fragment("compiler","local.cant.infer.null");

		public static Fragment KindnameTypeVariableBound(Object... args){
			return new Fragment("compiler","kindname.type.variable.bound",args);
		}

		public static final Fragment KindnameTypeVariableBound = new Fragment("compiler","kindname.type.variable.bound");

		public static Fragment ExceptionMessage(Object... args){
			return new Fragment("compiler","exception.message",args);
		}

		public static Fragment KindnameValue(Object... args){
			return new Fragment("compiler","kindname.value",args);
		}

		public static final Fragment KindnameValue = new Fragment("compiler","kindname.value");

		public static Fragment LocnModule_source_path(Object... args){
			return new Fragment("compiler","locn.module_source_path",args);
		}

		public static final Fragment LocnModule_source_path = new Fragment("compiler","locn.module_source_path");

		public static Fragment BadConstantValue(Object... args){
			return new Fragment("compiler","bad.constant.value",args);
		}

		public static Fragment DiamondInvalidArg(Object... args){
			return new Fragment("compiler","diamond.invalid.arg",args);
		}

		public static Fragment PackageNotVisible(Object... args){
			return new Fragment("compiler","package.not.visible",args);
		}

		public static Fragment KindnameModule(Object... args){
			return new Fragment("compiler","kindname.module",args);
		}

		public static final Fragment KindnameModule = new Fragment("compiler","kindname.module");

		public static Fragment MissingRetVal(Object... args){
			return new Fragment("compiler","missing.ret.val",args);
		}

		public static final Fragment MissingRetVal = new Fragment("compiler","missing.ret.val");

		public static Fragment WrongVersion(Object... args){
			return new Fragment("compiler","wrong.version",args);
		}

		public static Fragment BadClassFile(Object... args){
			return new Fragment("compiler","bad.class.file",args);
		}

		public static Fragment FatalErrNoJavaLang(Object... args){
			return new Fragment("compiler","fatal.err.no.java.lang",args);
		}

		public static final Fragment FatalErrNoJavaLang = new Fragment("compiler","fatal.err.no.java.lang");

		public static Fragment InvalidStaticInterface(Object... args){
			return new Fragment("compiler","invalid.static.interface",args);
		}

		public static Fragment BadStaticMethodInBoundLookup(Object... args){
			return new Fragment("compiler","bad.static.method.in.bound.lookup",args);
		}

		public static Fragment VerboseClasspath(Object... args){
			return new Fragment("compiler","verbose.classpath",args);
		}

		public static Fragment TokenFloat(Object... args){
			return new Fragment("compiler","token.float",args);
		}

		public static final Fragment TokenFloat = new Fragment("compiler","token.float");

		public static Fragment TokenDouble(Object... args){
			return new Fragment("compiler","token.double",args);
		}

		public static final Fragment TokenDouble = new Fragment("compiler","token.double");

		public static Fragment TokenIdentifier(Object... args){
			return new Fragment("compiler","token.identifier",args);
		}

		public static final Fragment TokenIdentifier = new Fragment("compiler","token.identifier");

		public static Fragment VerboseParsingStarted(Object... args){
			return new Fragment("compiler","verbose.parsing.started",args);
		}

		public static Fragment CapturedType(Object... args){
			return new Fragment("compiler","captured.type",args);
		}

		public static Fragment TypeReqRef(Object... args){
			return new Fragment("compiler","type.req.ref",args);
		}

		public static final Fragment TypeReqRef = new Fragment("compiler","type.req.ref");

		public static Fragment VerboseTotal(Object... args){
			return new Fragment("compiler","verbose.total",args);
		}

		public static Fragment DiamondNonGeneric(Object... args){
			return new Fragment("compiler","diamond.non.generic",args);
		}

		public static Fragment CantApplyDiamond1(Object... args){
			return new Fragment("compiler","cant.apply.diamond.1",args);
		}

		public static Fragment BadConstPoolTagAt(Object... args){
			return new Fragment("compiler","bad.const.pool.tag.at",args);
		}

		public static Fragment NotDefAccessDoesNotReadUnnamed(Object... args){
			return new Fragment("compiler","not.def.access.does.not.read.unnamed",args);
		}

		public static Fragment BadIntersectionTargetForFunctionalExpr(Object... args){
			return new Fragment("compiler","bad.intersection.target.for.functional.expr",args);
		}

		public static Fragment NotDefAccessPackageCantAccess(Object... args){
			return new Fragment("compiler","not.def.access.package.cant.access",args);
		}

	}
}